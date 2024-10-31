#!/usr/bin/env python3

import os
import sxtree

DEFAULT_TAB_WIDTH = 2


def noop(*_args, **_kwargs): pass


verbose = noop


class BuildError(sxtree.SxError):
    pass


class HeaderNode:
    def __init__(self, node):
        self.node = node
        self.level = node and int(node.code[1:]) or 0
        self.children = []


class GlobalValue:
    def __init__(self, *_args, **kwargs):
        self.html = kwargs.get('html')
        self.include = set()
        self.directory = []
        self.symbol = {}
        self.index = None
        self.header = []
        self.anchor_number = 0


class CodeTree:
    def __init__(self, stree, cls=None, **kwargs):
        self._class = cls if cls else CodeTree
        self.gval = kwargs.get('gval')
        self.symbol = self.gval.symbol

        paren = stree.paren
        code = None
        token = stree.token if stree.istoken() else None
        children = [self._class(c, **kwargs) for c in stree]
        if stree.isnode():
            token = None
            if children:
                code = children[0].token
                children = children[1:]

        self.paren = paren
        self.token = token
        self.code = code
        self.children = children
        self.expanded = False
        self.expand_pos = 0
        self.nbsp = code in ('code',)
        self.index = False
        self.anchor = False
        self.anchor_name = None
        self.anchor_text = None
        self.anchor_number = None

        self.build_code = None
        if code and code[0] == '@':
            if code in ('@h1', '@h2', '@h3', '@h4', '@h5', '@h6'):
                self.code = code[1:]
                self.anchor = True
            elif code == '@index':
                self.index = True
                self.gval.index = self
            elif code == '@def':
                self.build_code = self.build_code_def
                self.expand_pos = 1
            elif code == '@include':
                self.build_code = self.build_code_include

    def __repr__(self):
        return f'CodeTree(paren={self.paren}, token={self.token}, children={self.children})'

    def expand(self, nbsp=False):
        token = self.token
        tcode = token and token.code
        if nbsp and tcode == '"':
            text = token.replace(' ', '\xa0')
            token = token.update(token, text)
            self.token = token
        elif tcode == 'S':
            sym = self.symbol.get(token)
            if sym is not None:
                self.expanded = True
                return sym
        return [self]

    def expand_children(self, nbsp=False):
        children = self.children[self.expand_pos:]
        children = sum((c.expand(nbsp) for c in children), [])
        self.children = self.children[:self.expand_pos] + children

    def build(self):
        self.expand_children(self.nbsp)
        if self.index:
            self.gval.header = []
        if self.anchor:
            self.anchor_number = 1 + self.gval.anchor_number
            self.gval.anchor_number = self.anchor_number
            self.anchor_name = f'index-{self.anchor_number}'
            self.anchor_text = self.build_anchor_name()
            self.gval.header.append(self)
        return self.build_code() if self.build_code else None

    def build_code_def(self):
        if len(self.children) < 1:
            raise BuildError(self.code, 'パラメータが足りません')
        sym = self.children[0].token
        if sym in self.symbol:
            raise BuildError(self.code, f'識別子 "{sym}" は既にあります')
        if sym.code != 'S':
            raise BuildError(self.code, f'識別子 "{sym}" がありません')
        verbose(f'@def: {sym}')
        self.symbol[sym] = self.children[1:]
        return None

    def build_code_include(self):
        if len(self.children) < 1:
            raise BuildError(self.code, 'パラメータが足りません')
        sym = self.children[0].token
        if sym is None or sym.code != '"':
            raise BuildError(self.code, 'ファイル名がありません')
        verbose(f'@include: {sym}')

        incset = self.gval.include
        dirstk = self.gval.directory

        cwd = os.getcwd()
        os.chdir(dirstk[-1])
        file = os.path.abspath(sym)
        os.chdir(cwd)
        file = os.path.relpath(file)

        if file in incset:
            verbose(f'@include:skip: "{sym}"')
            stree = None
        else:
            incset.add(file)
            tree, fdir = sxtree.parse(file=file)
            dirstk.append(fdir)
            tree = self._class(tree, gval=self.gval)
            stree = tree.build()
            dirstk.pop()

        return stree

    def build_anchor_name(self):
        token = self.token or ''
        return token + ''.join(
            c.build_anchor_name() for c in self.children if c.attribute is None)


class HTMLTagInfo:
    DOCTYPE = '<!DOCTYPE html>'

    # 0: <tag> NEWLINE
    # 1: <tag/> NEWLINE
    # =: <tag> ... </tag> NEWLINE
    # *: <tag> NEWLINE
    #    INDENT ...
    #    </tag> NEWLINE

    TABLE = {
        '!DOCTYPE': set('=0'),
        #
        'html': set('*'),
        'head': set('*'),
        'meta': set('=1'),
        'title': set('='),
        'style': set('*'),
        #
        'body': set('*'),
        #
        'h1': set('='),
        'h2': set('='),
        'h3': set('='),
        'h4': set('='),
        'h5': set('='),
        'h6': set('='),
        #
        'blockquote': set('*'),
        #
        'table': set('*'),
        'tr': set('*'),
        'th': set('='),
        'td': set('='),
        #
        'dl': set('*'),
        'dd': set('='),
        #
        'ol': set('*'),
        'ul': set('*'),
        'li': set('='),
        #
        'p': set('='),
        'div': set('='),
        #
        'br': set('1'),
    }

    @staticmethod
    def lookup(mode):
        tags = HTMLTagInfo.TABLE
        return {t for t in tags if mode & tags[t]}


class HTML:
    ENCODE_MAP = {
        '&': '&amp;',
        '<': '&lt;',
        '>': '&gt;',
        '\xa0': '&nbsp;',
    }

    SINGLE = HTMLTagInfo.lookup({'0'})
    SINGLE_SLASH = HTMLTagInfo.lookup({'1'})

    NL_LINE = HTMLTagInfo.lookup({'='})
    NL_TAG = HTMLTagInfo.lookup({'*'})

    @staticmethod
    def encode(s):
        tab = HTML.ENCODE_MAP
        return ''.join(tab.get(c, c) for c in s)

    @staticmethod
    def anchor_name(s):
        r = ''
        for c in s:
            if not c.isalnum():
                c = ''.join(f'%{d:02x}' for d in c.encode())
            r += c
        return r

    def __init__(self, stree, *_args, **kwargs):
        self.gval = GlobalValue(html=self)
        self.tree = HTMLNode(stree, gval=self.gval)
        self.tab_width = kwargs.get('tab_width', DEFAULT_TAB_WIDTH)
        self.gval.directory.append(kwargs.get('directory', os.getcwd()))

    def build(self):
        tree = self.tree.build()
        index = self.build_header()

        html = []
        indent_level = 0
        indent = ''
        newline = False
        for code in tree:
            if code is None:
                continue
            if not isinstance(code, HTMLConn):
                html.append(code)
                newline = False
                continue
            if code.mode == '\n':
                if not newline:
                    html.append('\n')
                    newline = True
                continue
            if code.mode == '=':
                html.append(indent)
                continue
            if code.mode == '+':
                indent_level += 1
                indent = ' ' * self.tab_width * indent_level
                if not newline:
                    html.append('\n')
                newline = True
                continue
            if code.mode == '-':
                indent_level -= 1
                indent = ' ' * self.tab_width * indent_level
                if not newline:
                    html.append('\n')
                html.append(indent)
                newline = False
                continue
            if code.mode == '#':
                for iline in index:
                    html += [indent + iline]
                newline = True
                continue
            # BUG!?
        return ''.join(html)

    def build_header(self):
        index = []
        for node in self.gval.header:
            level = int(node.code[1:])
            index.append(''.join([
                '\xa0' * 4 * level,
                '<a href="#', node.anchor_name, '">',
                node.anchor_text,
                '</a><br/>\n',
            ]))
        return index


class HTMLConn:
    def __init__(self, mode):
        self.mode = mode

    def __str__(self):
        return ''

    def __repr__(self):
        return f'HTMLConn({repr(self.mode)})'


class HTMLNode(CodeTree):
    NEWLINE = HTMLConn('\n')
    INDENT = HTMLConn('=')
    ENTER = HTMLConn('+')
    LEAVE = HTMLConn('-')
    INDEX = HTMLConn('#')

    def __init__(self, stree, **kwargs):
        super().__init__(stree, cls=HTMLNode, **kwargs)
        self.attribute = None
        self.encode = True
        if self.build_code is None:
            self.build_code = self.build_tree
        if self.paren == '[]':
            self.attribute = self.code
            self.build_code = self.build_empty
            self.token = None
        elif self.code in HTML.SINGLE:
            self.build_code = self.build_single
        elif self.code in HTML.SINGLE_SLASH:
            self.build_code = self.build_single_slash
        elif self.code == 'style':
            self.build_code = self.build_style
        elif self.code == 'pre':
            self.build_code = self.build_pre
        elif self.code == '@pre':
            self.build_code = self.build_pre_children
        elif self.code == '@plain':
            self.code = None
            self.encode = False
            self.build_code = self.build_plain
        elif self.code == '@index':
            self.build_code = self.build_index

    def __repr__(self):
        attr = repr(str(self.attribute)) if self.attribute else repr(None)
        return f'HTML({super().__repr__()}, attribute={attr})'

    def get_tag_conn(self, tag):
        if tag in HTML.NL_TAG:
            return [self.INDENT, self.ENTER, self.LEAVE, self.NEWLINE]
        if tag in HTML.NL_LINE:
            return [self.INDENT, None, None, self.NEWLINE]
        return [None] * 4

    def build_attribute_text(self):
        token = self.token
        return token.replace('"', '\\"') if token else ''

    def build_attribute(self):
        self.expand_children()
        attr = self.attribute
        if attr is None:
            return ''
        if self.children:
            return ''.join([
                f' {attr}="',
                ''.join(c.build_attribute_text() for c in self.children),
                '"'
            ])
        return f' {attr}'

    def build_attributes(self):
        return ''.join(c.build_attribute() for c in self.children if c.attribute)

    def build_children(self):
        children = (c.build() for c in self.children if c.attribute is None)
        return sum(filter(None, children), [])

    def build_empty(self):
        return []

    def build_single(self, slash=''):
        tag = self.code
        sprev, _spost, _eprev, epost = self.get_tag_conn(tag)
        return [] if tag is None else [sprev, f'<{tag}{self.build_attributes()}{slash}>', epost]

    def build_single_slash(self):
        return self.build_single(slash='/')

    def build_tree(self):
        tag = self.code
        if tag:
            sprev, spost, eprev, epost = self.get_tag_conn(tag)
            attribute = self.build_attributes()
            children = self.build_children()

            apfx, asfx = None, None
            if self.anchor:
                aname = self.anchor_name
                apfx = f'<a name="{aname}">'
                asfx = '</a>'
            return [
                sprev, f'<{tag}{attribute}>', spost,
                apfx, *children, asfx,
                eprev, f'</{tag}>', epost
            ]
        token = self.token
        if token is not None:
            return [HTML.encode(token)] if self.encode else ['' + token]
        return self.build_children()

    def build_style(self):
        children = []
        for c in self.children:
            children += [self.INDENT, c.token, self.NEWLINE]
        return [
            self.INDENT, '<style>', self.NEWLINE,
            *children,
            self.INDENT, '</style>', self.NEWLINE
        ]

    def build_pre(self):
        return [
            self.INDENT, '<pre>',
            *self.build_pre_children(False),
            '</pre>', self.NEWLINE
        ]

    def build_plain(self):
        for child in self.children:
            if child.code is None:
                child.encode = False
        return self.build_tree()

    def build_pre_children(self, space=True):
        children = []
        sep, nsep = [], ['<br/>', self.NEWLINE]
        for child in [c.build() for c in self.children
                      if c.attribute is None]:
            if space:
                child = [(c.replace(' ', '\xa0') if isinstance(c, str) else c) for c in child]
            if len(child) == 1 and isinstance(child[0], str):
                children += sep
                sep = nsep
            else:
                sep = []
            children += filter(None, child)
        return children

    def build_index(self):
        return [*self.build_children(), self.NEWLINE, self.INDEX]


if __name__ == '__main__':
    import argparse
    import sys

    def main():
        global verbose

        parser = argparse.ArgumentParser()
        # parser.add_argument('-d', '--debug', action='store_true')
        parser.add_argument('-v', '--verbose', action='store_true')
        parser.add_argument('-t', '--tab-width', type=int, default=DEFAULT_TAB_WIDTH)
        parser.add_argument('--html', action='store_true')
        parser.add_argument('inpfile', metavar='INP')
        parser.add_argument('outfile', metavar='OUT')

        args = parser.parse_args()
        if args.verbose:
            verbose = print

        try:
            tree, fdir = sxtree.parse(file=args.inpfile)
        except sxtree.SxException as e:
            print(e)
            return 2

        output = None
        if args.html:
            html = HTML(tree, directory=fdir, tab_width=args.tab_width)

            try:
                output = html.build()
            except sxtree.SxException as e:
                print(e)
                return 2

        if output:
            with open(args.outfile, 'w') as fp:
                fp.write(output)
        return 0

    sys.exit(main())
