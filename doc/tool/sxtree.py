#!/usr/bin/env python3

import os
import sys


class SxException(Exception):
    def __init__(self, message, *args, **kwargs):
        super().__init__(message, *args, **kwargs)
        self.message = message


class SxError(SxException):
    def __init__(self, token, *args, **kwargs):
        super().__init__(''.join([f'{token.file}:{token.line}: ', *args]))


class Token(str):
    def __init__(self, *_args, **_kwargs):
        self.code = None
        self.file = None
        self.line = 0

    def __repr__(self):
        s = super().__repr__()
        code = repr(self.code)
        file = repr(self.file)
        line = repr(self.line)
        return f'Token(code={code}, file={file}, line={line}, str={s})'

    def __str__(self):
        s = super().__str__()
        return f'"{s}"' if self.code in '\'"' else s

    @staticmethod
    def create(code, file, line, text):
        token = Token(text)
        token.code = code
        token.file = file
        token.line = line
        return token

    @staticmethod
    def update(token, text):
        ntoken = Token(text)
        ntoken.code = token.code
        ntoken.file = token.file
        ntoken.line = token.line
        return ntoken


class Node(list):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.code = kwargs.get('code')
        self.paren = kwargs.get('paren')
        self.token = kwargs.get('token')

    def __repr__(self):
        children = f'{super().__repr__()}'
        return f'Node(code={self.code}, paren={self.paren}, token={self.token}, {children})'

    def isroot(self):
        return self.code == 'root'

    def isnode(self):
        return self.code == 'node'

    def istoken(self):
        return self.code == 'token'

    def dump(self):
        text = []
        for child in self:
            if child.istoken():
                text += [str(child.token)]
            else:
                text += [child.dump()]
        return text


class TextIter:
    def __init__(self, initer):
        self.initer = initer
        self.stack = []
        self.line = 1

    def __iter__(self):
        return self

    def __next__(self):
        c = self.stack.pop() if self.stack else next(self.initer)
        if c == '\n':
            self.line += 1
        return c

    def rewind(self, c):
        """巻き戻し"""

        self.stack.append(c)
        if c == '\n':
            self.line -= 1


class Lexer:
    OCTAL = '01234567'
    DECIMAL = '0123456789'
    HEXDECIMAL = '0123456789ABCDEFabcdef'

    PAREN = {'(', ')', '[', ']', '{', '}'}

    SPACE = set(chr(n) for n in range(33))
    SYMBOL = set(chr(n) for n in range(33, 0x80)) - {'"', ';'} - PAREN

    ESCAPE = {
        'a': '\a', 'b': '\b', 't': '\t', 'n': '\n',
        'v': '\v', 'f': '\f', 'r': '\r',
    }

    def __init__(self, *_args, **kwargs):
        file = '<text>'
        fdir = None
        text = kwargs.get('text')
        if text is None:
            file = kwargs.get('file')
            if not file:
                raise TypeError()
            if file == '-':
                text = sys.stdin.read()
                file = '<stdin>'
            else:
                subdir = '/' in file
                if subdir:
                    cwd = os.getcwd()
                    os.chdir(os.path.dirname(file))
                fdir = os.getcwd()
                with open(os.path.basename(file)) as fp:
                    text = fp.read()
                if subdir:
                    os.chdir(cwd)

        self.table = {'"': self._string}
        self.table |= {c: self._symbol for c in Lexer.SYMBOL}

        self.file = file
        self.text = TextIter(iter(text))
        self.token = self._generator()
        self.last_token = None
        self.file_directory = fdir if fdir else os.getcwd()

    def __iter__(self):
        return self

    def __next__(self):
        self.last_token = next(self.token)
        return self.last_token

    def _generator(self):
        for c in self.text:
            f = self.table.get(c)
            if f:
                yield f(c)
            elif c in self.SPACE:
                self._space()
            elif c in self.PAREN:
                yield self._token(c, c)
            elif c == ';':
                self._comment(c)
            else:
                e = f'"{c}"' if c != '"' else f'({c})'
                raise self._error(f'未知の文字 {e}')

    def _space(self):
        for c in self.text:
            if not c in self.SPACE:
                self.text.rewind(c)
                break

    def _symbol(self, s):
        sym = self.SYMBOL
        text = s
        for c in self.text:
            if not c in sym:
                self.text.rewind(c)
                break
            text += c
        return self._token('S', text)

    def _string(self, s):
        text = ''
        for c in self.text:
            if c == s:
                return self._token(s, text)
            if c == '\\':
                text += self._escape()
            else:
                text += c
        raise self._error(f'文字列の終端({s})がありません')

    def _escape(self):
        try:
            c = next(self.text)
            if c in 'Xx':
                return self._escape_byte()
            if c in 'Uu':
                return self._escape_ucs()
            if c in self.OCTAL:
                return self._escape_oct(c)
            return self.ESCAPE.get(c, c)
        except StopIteration:
            pass
        raise self._error()

    def _escape_byte(self):
        return self._escape_code('', 2, 16, self.HEXDECIMAL)

    def _escape_ucs(self):
        return self._escape_code('', 6, 16, self.HEXDECIMAL)

    def _escape_oct(self, v):
        return self._escape_code(v, 2, 8, self.OCTAL)

    def _escape_code(self, v, n, base, cset):
        for _ in range(n):
            c = next(self.text)
            if not c in cset:
                self.text.rewind(c)
                break
            v += c
        if v:
            return chr(int(v, base))
        raise self._error('文字コードがありません')

    def _comment(self, _c):
        for c in self.text:
            if c == '\n':
                break

    def _token(self, code, text):
        return Token.create(code, self.file, self.text.line, text)

    def _error(self, *args):
        return SxException(''.join([f'{self.file}:{self.text.line}: ', *args]))


class Parser(Lexer):
    PAREN_MAP = {'(': ')', '[': ']', '{': '}'}

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)

        self.node_stack = []
        self.current_node = Node(code='root')
        self.paren_stack = []
        self.current_paren = None

    def parse(self):
        for token in self:
            pbeg = token.code
            pend = self.PAREN_MAP.get(pbeg)
            if pend:
                self.parse_block(pbeg, pend)
            else:
                raise SxError(token, '未知の構文です')
        return (self.current_node, self.file_directory)

    def parse_block(self, pbeg, pend):
        self.node_stack.append(self.current_node)
        self.current_node = Node(code='node', paren=pbeg+pend)
        self.paren_stack.append(self.current_paren)
        self.current_paren = self.last_token
        for token in self:
            cbeg = token.code
            if cbeg == pend:
                self.node_stack[-1].append(self.current_node)
                self.current_node = self.node_stack.pop()
                self.current_paren = self.paren_stack.pop()
                return
            cend = self.PAREN_MAP.get(cbeg)
            if cend:
                self.parse_block(cbeg, cend)
                continue
            self.current_node.append(Node(code='token', token=token))
        raise SxError(self.current_paren, f'"{pend}" がありません')


def parse(*args, **kwargs):
    return Parser(*args, **kwargs).parse()
