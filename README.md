# wasmgen - WebAssembly 用アセンブラ

[WebAssembly](https://webassembly.github.io/spec/core/)の[バイナリ形式(.wasm)](https://webassembly.github.io/spec/core/binary/)を生成するための1行1命令形式アセンブラです。

対応しているファイルの文字コードは UTF-8/16/32(+BOM) です。

他のアセンブラとの互換性はありません。[テキスト形式(.wat)](https://webassembly.github.io/spec/core/text/)よりも冗長になります。

数値のオーバーフローや .wasm 適合などのチェック処理は行っていませんので、記述には十分注意してください。

生成された .wasm 内容の詳細を確認するために[wasmdump](https://github.com/ikiuo/WebAssembly-wasmdump)を用意しています。


# wasmgen のビルド

ソースコードは C++14 を前提として記述しています。


## Unix 系

[GNU make](https://www.gnu.org/software/make/) を使ってビルドします。コンパイラは [clang++](https://clang.llvm.org/) を使用しています。


## Windows

VisualStudio (2022) では wasmgen.vcxproj を使ってビルドしてください。


# アセンブラの表記

各行は以下の形式で記述します。

```
[ラベル[:]] 命令  [オペランド1 [, オペランド2...]]
```

<code>ラベル</code>は<code>命令</code>に応じて値が設定されます。ラベル直後に<code>:</code>を置く事ができます。<code>命令</code>には、実行命令と疑似命令があります。<code>オペランド</code>は<code>命令</code>に応じた内容を記述します。コンマ(<code>,</code>)区切りです。


### コメント

<code>#</code>, <code>;</code>, <code>//</code> から行末まで無視されます。

```
# コメント1↲
; コメント2↲
// コメント3↲
```

<code>/\*</code> から <code>\*/</code> まで無視されます。

```
... /* コメント */ ...
```


### 行の結合

行末が<code>\\</code>ならば改行文字を空白とします。これにより次の行を結合した一行として処理されます。

#### 行末が<code>\\</code>文字の例(その1)

```
label:  operator    operand1, operand2,\↲
                    operand3, operand4↲
```

改行が空白になるので、次の表記と同じ扱いになります。

```
label:  operator    operand1, operand2,                     operand3, operand4↲
```


#### 行末が<code>\\</code>文字の例(その2)

```
;# コメント\↲
   この行もコメント↲
```

改行が空白になるので、次の表記と同じ扱いになります。

```
;# コメント    この行もコメント↲
```


## 数値

小数点(<code>.</code>)は一ヶ所、桁区切りとして(<code>_</code>)が使用可能です。

| 基数 | 文字 | 備考 |
|:-:|:--|:--
| 10 | 先頭: 1 2 3 4 5 6 7 8 9 <br/> 後続: 0 1 2 3 4 5 6 7 8 9 <br/> 指数: e <br/> 符号: - + (なし) <br/> 後続: 0 1 2 3 4 5 6 7 8 9 | 1234567890 <br/> 1.97625e-2 = 1.97625 / 100 |
| 16 | 先頭: 0x <br/> 後続: 0 1 2 3 4 5 6 7 8 9 a b c d e f <br/> 指数: p <br/> 符号: - + (なし) <br/> 後続: 0 1 2 3 4 5 6 7 8 9 | 0x89ab\_cdef <br/> 0x0123p4 = 0x0123 << 4 |
| 8 | 先頭: 0 <br/> 後続: 0 1 2 3 4 5 6 7 | 0377 |
| 2 | 先頭: 0b <br/> 後続: 0 1 | 0b1110\_0110 |


## 演算子


### 数値用演算子

| 種類 | 演算子 | 例 |
|:-:|:-:|:--|
| 単項 | <code>+</code> <br/> <code>-</code> <br/> <code>!</code> <br/> <code>~</code> | <code>+</code> value <br/> <code>-</code> value <br/> <code>!</code> value <br/> <code>~</code> value |
| 二項(積) | <code>*</code> <br/> <code>/</code> <br/> % | val1 <code>*</code> val2 <br/> val1 <code>/</code> val2 <br/> val1 <code>%</code> val2 |
| 二項(和) | <code>+</code> <br/> <code>-</code> <br/> | val1 <code>+</code> val2 <br/> val1 <code>-</code> val2 |
| 二項(シフト) | <code>&lt;&lt;</code> <br/> <code>&gt;&gt;</code> <br/> <code>&gt;&gt;&gt;</code> | val1 <code>&lt;&lt;</code> val2 <br/> val1 <code>&gt;&gt;</code> val2 <br/> val1 <code>&gt;&gt;&gt;</code> val2 |
| 二項(比較) | <code>&lt;</code> <br/> <code>&lt;=</code> <br/> <code>&gt;=</code> <br/> <code>&gt;</code> | val1 <code>&lt;</code> val2 <br/> val1 <code>&lt;=</code> val2 <br/> val1 <code>&gt;=</code> val2 <br/> val1 <code>&gt;</code> val2 |
| 二項(比較) | <code>==</code> <br/> <code>!=</code> | val1 <code>==</code> val2 <br/> val1 <code>!=</code> val2 |
| 二項(論理) | <code>&amp;</code> <br/> <code>\|</code> <br/> <code>^</code> | val1 <code>&amp;</code> val2 <br/> val1 <code>\|</code> val2 <br/> val1 <code>^</code> val2 |
| 二項(真偽) | <code>&amp;&amp;</code> <br/> <code>\|\|</code> | val1 <code>&amp;&amp;</code> val2 <br/> val1 <code>\|\|</code> val2 |
| 三項(選択) | <code>? :</code> | val1 <code>?</code> val2 <code>:</code> val3 |


### リスト

括弧<code>(),[],{}</code>の間に情報を<code>,</code>区切りで記述します。

**例**

```
(1,2,3)
[a,b,c]
{x,y,z}
```


### リストの内部展開

リスト内で、リストの前に<code>*</code>を記述すると、単一項から親リストの要素列に展開されます。オペランドは括弧なしのリストになっています。

**例**

```
(1,*(2,3)) → (1,2,3)
```


### リスト演算

リストとリストの結合を演算子<code>+</code>で行うことができます。

**例**

```
[1,2,3] + [4,5,6] → [1,2,3,4,5,6]
```

リストの繰り返し複製を演算子<code>\*</code>で行うことができます。

**例**

```
[1,2]*3 → [1,2,1,2,1,2]
```


## 文字列

文字<code>"</code>または<code>'</code>で括られた範囲を文字列とします。文字<code>\\</code>をエスケープ文字とします。

| 種類 | エスケープ シーケンス |
|:--|:--|
| 制御文字 | <code>\\a</code> <code>\\b</code> <code>\\t</code> <code>\\n</code> <code>\\v</code> <code>\\r</code> |
| 8進コード | 最大3桁(8bit) : <code>\\</code>NNN |
| 16進コード | 最大2桁(8bit) : <code>\\x</code>HH |
| UCSコード | 最大6桁(24bit) : <code>\\u</code>HHHHHH |
| 上記以外 | <code>\\</code> の直後の文字が有効 <br/> 例: <code>\\\\</code> → <code>\\</code>  |


### 文字列演算

文字列と文字列の結合を演算子<code>+</code>で行うことができます。

**例**

```
"abc" + "def" → "abcdef"
```

文字列の繰り返し複製を演算子<code>\*</code>で行うことができます。

**例**

```
"ab"*3 → "ababab"
```


## 識別子


### 命令用の識別子

先頭の文字は、アルファベット

```
A B C D E F G H I J K L M N O P Q R S T U V W X Y Z
a b c d e f g h i j k l m n o p q r s t u v w x y z
```

に加えて

```
$ . _ @
```

が使用可能です。2文字目以降は数字

```
0 1 2 3 4 5 6 7 8 9
```

が加わります。先頭の文字が <code>.</code> または <code>@</code> で始まる識別子はアセンブラ用とします。


### ラベル用の識別子

命令用と同じですが、(疑似)命令以外の識別子が行の先頭(空白を除く)に現れたときにラベルと判断します。


## 値型 : numtype / vectype / reftype / valtype

引数型および返値型は以下になります。

| 種類 | 型 |
|:-:|:--|
| numtype | i32 i64 f32 f64 |
| vectype | v128 |
| reftype | funcref externref |
| valtype | numtype:<br/>&nbsp;&nbsp;&nbsp; i32 i64 f32 f64 <br/> vectype:<br/>&nbsp;&nbsp;&nbsp; v128 <br/> reftype:<br/>&nbsp;&nbsp;&nbsp; funcref externref |

numtype と vectype では同型で符号の扱いが存在する表現があります。

| 共通型 | 符号あり | 符号なし |
|:--|:--|:--|
| i32 | s32 | u32 |
| i64 | s64 | u64 |
| i8x16 <br/> i16x8 <br/> i32x4 <br/> i64x2 <br/> &nbsp;↑ <br/> v128 | s8x16 <br/> s16x8 <br/> s32x4 <br/> s64x2 <br/> &nbsp; <br/> &nbsp; | u8x16 <br/> u16x8 <br/> u32x4 <br/> u64x2  <br/> &nbsp; <br/> &nbsp; |


# 疑似命令


## マクロ疑似命令

### <code>.defmacro</code>および<code>.endmacro</code>疑似命令

```
マクロ名        .defmacro       引数1, 引数2, ...
　　　　        ;# この行から
　　　　        ;#    .endmacro の直前行までを
　　　　        ;#       (@で始まる疑似命令やマクロ疑似命令を除く)
　　　　        ;#    <マクロ名> 命令として展開する
　　　　        ;#    <引数1>,<引数2>,... は置換される情報
　　　　        ;# この行まで
　　　　        .endmacro       ;# マクロ定義終わり
```

<code>マクロ名</code>で<code>.defmacro</code>の次の行から<code>.endmacro</code>直前の行までをマクロを登録します。


### <code>.delmacro</code>疑似命令

```
            .delmacro       マクロ名
```

登録されている<code>マクロ名</code>を削除します。


### 使用例

```
;# マクロ定義

$i32.inc    .defmacro       labelidx
            local.get       labelidx
            i32.const       1
            i32.add
            local.set       labelidx
            .endmacro

;# マクロ使用

func1       .code
n           .local          i32
            ;# ...
            $i32.inc        n
            ;# ここに
            ;#   local.get  n
            ;#   i32.const  1
            ;#   i32.add
            ;#   local.set  n
            ;# が展開される
            ;# ...

;# マクロ削除

            .delmacro       $i32.inc
            ;# 以下では $i32.inc は使用できない

            ;# マクロ削除では以下のパターンに注意
            ;# 1) .defmacro で引数なしを定義
            ;# 2) .delmacro で削除
            ;# 3) 削除されたマクロ名のみの行を記述

mtest       .defmacro
            ;# ...
            .endmacro

            ;# ...
            mtest           ;# これはマクロ展開される
            ;# ...

            .delmacro       mtest

            ;# ...
            mtest           ;# これはラベル扱い
            ;# ...
```


## <code>.alias</code>疑似命令

```
name        .alias          value...
```

<code>name</code>を<code>value...</code>として扱います。


## セクション用疑似命令

[WebAssembly モジュール](https://webassembly.github.io/spec/core/binary/modules.html)の各セクションに対応するための疑似命令があり、ラベルは序数として登録されます。

| セクション | 疑似命令 | ラベル登録先 |
|--:|:--|:--|
| Custom Section | .custom | |
| Type Section | .type | typeidx |
| Import Section | .import.func <br/> .import.table <br/> .import.memory <br/> .import.const <br/> .import.var | funcidx <br/> tableidx <br/> memidx <br/> globalidx <br/> globalidx |
| Function Section | | |
| Global Section | .gconst <br/> .gvar | globalidx <br/> globalidx |
| Export Section | .export | |
| Start Section | .start | |
| Element Section | .element | elemidx |
| Code Section | .code | funcidx |
| Data Section | .data | dataidx |
| Data Count Section | | |


## Type Section

1つの関数型データは

+ 引数型リスト
+ 返値型リスト

で構成され、Type Section は関数型のリストになります。


### <code>.type</code>疑似命令

```
関数型名    .type       <返値の型:valtype>リスト
```

<code>.type</code>疑似命令で関数型を開始します。<code>関数型名</code>は名前空間<code>typeidx</code>に序数として登録され、<code>typeidx</code>を指定する場面で参照できます。


### <code>.param</code>疑似命令

```
引数名      .param      <引数の型:valtype>
```

<code>.type</code>疑似命令の下に、<code>.param</code>疑似命令を並べて引数型リストにします。<code>引数名</code>は関数の名前空間<code>関数型名</code>の<code>localidx</code>に序数として登録され、<code>localidx</code>を指定する場面で参照できます。


### <code>.type</code>例

```
;# C言語の typedef void (*ftype0)(i32 x); に相当
ftype0      .type
x           .param      i32

;# C言語の typedef i64 (*ftype1)(i64 x, i64 y); に相当
ftype1      .type       i64
x           .param      i64
y           .param      i64

;# C言語の typedef double (*ftype2)(double x, double y, double z); に相当
ftype2      .type       f64
x           .param      f64
y           .param      f64
z           .param      f64
```


## Import Section

1つの外部参照用データは

+ 外部モジュール名
+ 参照名
+ 種類
  + ①関数
  + ②テーブル
  + ③メモリ
  + ④グローバル
    + 固定値
    + 可変値

で構成され、Import Section は外部参照リストになります。


### ① 関数 : <code>.import.func</code>疑似命令 (その1)

```
関数名      .import.func    外部モジュール名, 外部関数名, <typeidx>
```

<code>.import.func</code>疑似命令で外部関数の呼び出し情報を生成します。<code>関数名</code>は名前空間<code>funcidx</code>に序数として登録され、<code>funcidx</code>を指定する場面で参照できます。


■ <code>.import.func</code>例

```
func0       .import.func    "xmod", "i64add", ftype1
```


### ① 関数 : <code>.import.func</code>疑似命令 (その2)

<code>&lt;typeidx&gt;</code>部分を <code>{[引数型リスト],[返値型リスト]}</code> に置換した形式を使用できます。置換した関数型情報は自動的に Type Section に登録されます。(<code>.type</code>疑似命令での記述が不要)

■ <code>.import.func</code>例

```
func1       .import.func    "xmod", "i32add", {[i32,i32],[i32]}
```


### ② テーブル : <code>.import.table</code>疑似命令

```
テーブル名   .import.func    外部モジュール名, 外部テーブル名, <reftype>, 最小長 [, 最大長]
```

<code>.import.table</code>疑似命令で外部テーブルの参照情報を生成します。<code>テーブル名</code>は名前空間<code>tableidx</code>に序数として登録され、<code>tableidx</code>を指定する場面で参照できます。<code>最大長</code>は省略可です。

■ <code>.import.table</code>例

```
tab0        .import.table   "xmod", "table1", funcref, 4
```


### ③ メモリ : <code>.import.memory</code>疑似命令

```
メモリ名    .import.memory  外部モジュール名, 外部メモリ名, 最小長 [, 最大長]
```

<code>.import.memory</code>疑似命令で外部バイト列の参照情報を生成します。<code>メモリ名</code>は名前空間<code>memidx</code>に序数として登録され、<code>memidx</code>を指定する場面で参照できます。<code>最大長</code>は省略可です。

■ <code>.import.memory</code>例

```
mem0        .import.memory  "xmod", "memory1", 8
```


### ④ グローバル(固定値) : <code>.import.const</code>疑似命令

```
グローバル名  .import.const   外部モジュール名, 外部変数名, <valtype>
```

<code>.import.const</code>疑似命令で外部変数の参照情報を生成します。<code>グローバル名</code>は名前空間<code>globalidx</code>に序数として登録され、<code>globalidx</code>を指定する場面で参照できます。

■ <code>.import.const</code>例

```
global0     .import.const   "xmod", "gconst1", i32
```


### ④ グローバル(可変値) : <code>.import.var</code>疑似命令

```
グローバル名  .import.var     外部モジュール名, 外部変数名, <valtype>
```

<code>.import.const</code>疑似命令と同じですが、内容が変更可能です。

■ <code>.import.var</code>例

```
global1     .import.var     "xmod", "gvar1", i64
```

### ※ <code>.import</code>疑似命令(規定の外部モジュール名を指定)

Import Section の例を集めると

```
func0       .import.func    "xmod", "i64add", ftype1
func1       .import.func    "xmod", "i32add", {[i32,i32],[i32]}
tab0        .import.table   "xmod", "table1", funcref, 4
mem0        .import.memory  "xmod", "memory1", 8
global0     .import.const   "xmod", "gconst1", i32
global1     .import.var     "xmod", "gvar1", i64
```

になり、外部モジュール名 "xmod" の重複は省略したくなります。<code>.import</code> 疑似命令を使用して、<code>.import.</code>を<code>.imp.</code>に置換すると、"xmod" は最初だけになります。

```
            .import     "xmod"  ;# .imp.* 疑似命令の外部モジュール名を設定
func0       .imp.func   "i64add", ftype1
func1       .imp.func   "i32add", {[i32,i32],[i32]}
tab0        .imp.table  "table1", funcref, 4
mem0        .imp.memory "memory1", 8
global0     .imp.const  "gconst1", i32
global1     .imp.var    "gvar1", i64
```


## Table Section

1つのテーブル領域は

+ 参照型(reftype)
+ テーブル長 : 最小
+ テーブル長 : 最大 (省略可)

で構成されています。

### <code>.table</code>疑似命令

```
テーブル名   .table      <reftype>, 最小長 [, 最大長]
```

<code>.table</code>疑似命令で、モジュールのテーブル領域を定義します。<code>テーブル名</code>は名前空間<code>tableidx</code>に序数として登録され、<code>tableidx</code>を指定する場面で参照できます。<code>最大長</code>は省略可です。

### <code>.table</code>例

```
tab1        .table      funcref, 4
tab2        .table      funcref, 8, 16
```


## Memory Section

1つのメモリ領域は

+ 領域長 : 最小
+ 領域長 : 最大 (省略可)

で構成され、Memory Section はメモリ領域のリストになっています。


### <code>.memory</code>疑似命令

```
メモリ名     .memory     最小長 [, 最大長]
```

<code>.memory</code>疑似命令で、モジュールのメモリ領域を定義します。<code>メモリ名</code>は名前空間<code>memidx</code>に序数として登録され、<code>memidx</code>を指定する場面で参照できます。<code>最大長</code>は省略可です。

### <code>.memory</code>例

```
mem1        .memory      4
mem2        .memory      8, 16
```


## Global Section

1つのグローバル変数は

+ データ型(valtype)
+ データ値を返すプログラム

で構成され、Global Section はグローバル変数のリストになっています。

<code>.gconst</code>または<code>.gvar</code>疑似命令でグローバル変数を開始します。


### <code>.gconst</code>疑似命令(固定値)

```
;# データ値を返すプログラムを記述する形式
グローバル名  .gconst     <valtype>
　　　　　　  ;# 以下、データ値を返すコード
　　　　　　  ;#   <valtype>.const   0
　　　　　　  ;# などを記述する

;# データ値を返すプログラムを自動生成する形式
グローバル名  .gconst     <valtype>, 値
　　　　　　  ;# データ値を返すコード
　　　　　　  ;#   <valtype>.const   値
　　　　　　  ;# が自動生成される
```

<code>グローバル名</code>は名前空間<code>globalidx</code>に序数として登録され、<code>globalidx</code>を指定する場面で参照できます。

■ <code>.gconst</code>例

```
global2     .gconst     i32, 0
global3     .gconst     f32, 1.0
global4     .gconst     f32x4, 1.0, 2.0, 3.0, 4.0

global5     .gvar       i32
            i32.const   5

global6     .gconst     f64
            f64.const   3.141592653589793
```


### <code>.gvar</code>疑似命令(可変値)

<code>.gconst</code>疑似命令と同様ですが、内容が変更可能です。

```
;# データ値を返すプログラムを記述する形式
グローバル名  .gvar       <valtype>
　　　　　　  ;# 以下、データ値を返すコードを記述

;# データ値を返すプログラムを自動生成する形式
グローバル名  .gvar       <valtype>, 値
```

■ <code>.gvar</code>例

```
global5     .gvar       i32, 0
global6     .gvar       f64, 2.0
global7     .gvar       f64x2, 0.5, 0.25
```


## Element Section

1つのテーブル要素は

+ 参照型(<code>reftype</code>)
+ モード
  + ①<code>declarative</code>
  + ②<code>passive</code>
  + ③<code>active</code>
    + テーブル位置(<code>tableidx</code>と<code>オフセット</code>)
+ 要素
  + Ⓐ<code>funcidx</code>のリスト
  + Ⓑ<code>reftype</code>を返すプログラムのリスト

で構成され、Element Section はテーブル要素のリストになっています。


### <code>.element</code>疑似命令

<code>.element</code>疑似命令でテーブル要素を開始します。<code>要素名</code>は名前空間<code>elemidx</code>に序数として登録され、<code>elemidx</code>を指定する場面で参照できます。


#### ①<code>declarative</code>モード

```
要素名      .element    <reftype>, declarative

;# <reftype>を別途記述する形式
要素名      .element    declarative
```


#### ②<code>passive</code>モード

```
要素名      .element    <reftype>, passive

;# <reftype>を別途記述する形式
要素名      .element    passive
```


#### ③<code>active</code>モード

```
要素名      .element    <reftype>, active, <tableidx>, オフセット

;# オフセットを別途記述する形式
要素名      .element    <reftype>, active, <tableidx>

;# <tableidx>とオフセットを別途記述する形式(1)
要素名      .element    <reftype>, active

;# <tableidx>とオフセットを別途記述する形式(2)
要素名      .element    <reftype>   ;# 既定で active モード

;# <reftype>と<tableidx>とオフセットを別途記述する形式
要素名      .element                ;# 既定で active モード
```


### <code>.reftype</code>疑似命令

```
            .reftype    <reftype>
```

<code>.element</code>疑似命令で<code>reftype</code>を省略したときに使用します。<code>.reftype</code>疑似命令もない場合は、<code>reftype</code>は<code>funcref</code>になります。


### <code>.tableidx</code>疑似命令

```
            .tableidx   <tableidx>
```

<code>.element</code>疑似命令で<code>tableidx</code>を省略したときに使用します。<code>.tableidx</code>疑似命令もない場合は、<code>tableidx</code>は<code>0</code>になります。


### <code>.offset</code>疑似命令

<code>.element</code>疑似命令で<code>オフセット</code>を省略したときに使用します。<code>.offset</code>疑似命令もない場合は、<code>オフセット</code>は<code>0</code>になります。

#### オフセット値を返すプログラムを記述する形式

```
            .offset
            ;# 以下、オフセット値を返すコード
            ;#   i32.const   値
            ;# などを記述する
```


#### オフセット値を返すプログラムを自動生成する形式

```
            .offset     値
            ;# オフセット値を返すコード
            ;#   i32.const   値
            ;# が自動生成される
```


### <code>.funcidx</code>疑似命令

```
            .funcidx    <funcidx>,<funcidx>,...
```

<code>.funcidx</code>疑似命令のオペランドには、<code>funcidx</code>のリストを記述します。また、<code>.funcidx</code>疑似命令は、<code>reftype</code>を返すプログラムの開始にもなります。


#### Ⓐ<code>funcidx</code>のリスト

```
            .funcidx    <funcidx>,<funcidx>,...
            .funcidx    <funcidx>,<funcidx>,...
            ;# ...
```

<code>.funcidx</code>疑似命令の下に、<code>ref.func</code>や<code>ref.null</code>などのコードが一切ない場合は、<code>funcidx</code>のリスト形式になります。


#### Ⓑ<code>reftype</code>を返すプログラムのリスト

```
            .funcidx    <funcidx>,<funcidx>,...
            ;# ここに funcidx を返すプログラム
            ;#   ref.null     <reftype>
            ;# などが入る
            .funcidx    <funcidx>,<funcidx>,...
            ;# オペランドの各 <funcidx> は
            ;#   ref.func     <funcidx>
            ;# に置換される
            ;# ...
```

<code>.funcidx</code>疑似命令の下に、<code>ref.func</code>や<code>ref.null</code>などのコードが1つでもあると、<code>.funcidx</code>疑似命令のオペランド<code>&lt;funcidx&gt;</code>は<code>ref.func &lt;funcidx&gt;</code>の自動的生成コードに置換されます。


### <code>.element</code>例

```
;# mode active その1
elem0       .element
            .reftype    funcref
            .tableidx   tab1
            .offset     0
            .funcidx    func2, func3

;# mode active その2
elem1       .element
            .tableidx   tab1
            .offset     1
            .funcidx    func2, func3

;# mode active その3
elem2       .element    active, tab1
            .offset     2
            .funcidx    func2, func3

;# mode active その4
elem3       .element    active, tab1, 3
            .funcidx    func2

;# mode passive
elem4       .element    passive
            .funcidx    func2
            ref.func    func3
            .funcidx
            ref.func    func4

;# mode declarative
elem4       .element    declarative
            .funcidx    func2, func3
```


## Data Section

1つのデータ列は

+ モード
  + ①<code>active</code>
    + テーブル位置(<code>memidx</code>と<code>オフセット</code>)
  + ②<code>passive</code>
+ データ列

で構成され、Data Section はテーブル列のリストになっています。


### <code>.data</code>疑似命令

<code>.data</code>疑似命令でデータ列を開始します。<code>データ名</code>は名前空間<code>dataidx</code>に序数として登録され、<code>dataidx</code>を指定する場面で参照できます。


#### ①<code>active</code>モード

```
データ名    .data       <memidx>, オフセット

;# オフセットを別途記述する形式
データ名    .data       <memidx>
            ;# 以下で
            ;#   .offset   [オフセット]
            ;# を使用する

;# <memidx>とオフセットを別途記述する形式
データ名    .data
            ;# 以下で
            ;#   .memidx   <memidx>
            ;#   .offset   [オフセット]
            ;# を使用する
```


#### ②<code>passive</code>モード

```
データ名    .data
```

<code>passive</code>モードでは、<code>.memidx</code>疑似命令と<code>.offset</code>疑似命令を使わないこと。


### <code>.memidx</code>疑似命令

```
            .memidx     <memidx>
```

<code>.data</code>疑似命令で<code>memidx</code>を省略したときに使用します。<code>.memidx</code>疑似命令もない場合は、<code>memidx</code>は<code>0</code>になります。


### <code>.offset</code>疑似命令

<code>.elment</code>疑似命令での<code>.offset</code>疑似命令と同様です。


### データ列用疑似命令

```
            .i8         <8ビット値>,<8ビット値>,...
            .i16        <16ビット値>,<16ビット値>,...
            .i32        <32ビット値>,<32ビット値>,...
            .i64        <64ビット値>,<64ビット値>,...
            .f32        <32ビット浮動小数点値>,<32ビット浮動小数点値>,...
            .f64        <64ビット浮動小数点値>,<64ビット浮動小数点値>,...
            .sleb       <符号あり整数値>,<符号あり整数値>,...       ;# 符号あり LEB128 形式
            .uleb       <符号なし整数値>,<符号なし整数値>,...       ;# 符号なし LEB128 形式
            .nstring    "uleb長の後に", "UTF-8", "文字列", ...
            .string     "UTF-8", "文字列", ...                  ;# null文字終端ではない
```

### エンディアン違いのデータ列用疑似命令

| データ型 | i8値 | i16値 | i32値 | i64値 | f32値 | f64値 |
|:-:|:--|:--|:--|:--|:--|:--|
| リトル エンディアン | <code>.i8</code> | <code>.i16</code> | <code>.i32</code> | <code>.i64</code> | <code>.f32</code> | <code>.f64</code> |
| リトル エンディアン | <code>.i8le</code> | <code>.i16le</code> | <code>.i32le</code> | <code>.i64le</code> | <code>.f32le</code> | <code>.f64le</code> |
| ビッグ エンディアン | <code>.i8be</code> | <code>.i16be</code> | <code>.i32be</code> | <code>.i64be</code> | <code>.f32be</code> | <code>.f64be</code> |


### <code>.data</code>例

```
data1       .data       mem1
            .i8         0,1

data2       .data       mem1, 2
            .i16        0x0302
```


## Code Section

1つの関数は

+ ローカル変数リスト
+ 実行コード

で構成され、Code Section は関数のリストになります。



### <code>.code</code>疑似命令

```
;# 関数型を指定する形式(A)
関数名      .code       <typeidx>
            ;# .param 疑似命令は使用できません

;# 返値型を指定する形式(B)
関数名      .code       <返値の型:valtype>, <返値の型:valtype>...
            ;# 引数は .param 疑似命令を使用します
```

<code>.code</code>疑似命令で関数(コード)を開始します。<code>関数名</code>は名前空間<code>funcidx</code>に序数として登録され、<code>funcidx</code>を指定する場面で参照できます。返値型を指定する形式(B)では、関数と関数型の記述を一ヶ所で行うので、<code>関数名</code>が名前空間<code>typeidx</code>にも序数として登録されます。


### <code>.param</code>疑似命令

```
引数名      .param      <引数の型:valtype>
```

Type Section の<code>.param</code>疑似命令と同じです。


### <code>.local</code>疑似命令

```
ローカル名  .local      <変数型:valtype>
```

<code>ローカル名</code>は名前空間<code>関数名</code>の<code>localidx</code>に序数として登録され、<code>localidx</code>を指定する場面で参照できます。ローカルの序数は、型(<code>valtype</code>)の種類で纏められるので順序が前後することがあります。


### <code>.code</code>例

```
;# 関数型を指定
func3       .code       ftype0
            local.get   x
            drop
            ;# 不足分の end 命令は自動追加

;# 関数型も記述
func4       .code       i32
x           .param      i32
            local.get   x
            end
```


## Start Section

<code>.start</code>疑似命令でモジュールの初期化処理関数を指定します。

```
            .start      <funcidx>
```

### <code>.start</code>例

```
            .start      func3
```



## Export Section

1つの開示用データは

+ 開示名
+ 種類
  + ①関数
  + ②テーブル
  + ③メモリ
  + ④グローバル

で構成され、Export Section は開示リストになります。

### <code>.export, .export.*</code>疑似命令

```
;# ①関数を開示する場合
            .export         <funcidx>, 開示名
            .export         <funcidx>  ;# 開示名は検索名<funcidx>を使用
            .export.func    <funcidx>, 開示名
            .export.func    <funcidx>  ;# 開示名は検索名<funcidx>を使用

;# ②テーブルを開示する場合
            .export         <tableidx>, 開示名
            .export         <tableidx>  ;# 開示名は検索名<tableidx>を使用
            .export.table   <tableidx>, 開示名
            .export.table   <tableidx>  ;# 開示名は検索名<tableidx>を使用

;# ③メモリを開示する場合
            .export         <memidx>, 開示名
            .export         <memidx>  ;# 開示名は検索名<memidx>を使用
            .export.memory  <memidx>, 開示名
            .export.memory  <memidx>  ;# 開示名は検索名<memidx>を使用

;# ④グローバル変数を開示する場合
            .export         <globalidx>, 開示名
            .export         <globalidx>  ;# 開示名は検索名<globalidx>を使用
            .export.global  <globalidx>, 開示名
            .export.global  <globalidx>  ;# 開示名は検索名<globalidx>を使用
```

<code>.export</code>疑似命令は、<code>globalidx,funcidx,tableidx,memidx</code>の順にラベル名を検索します。同一名が複数のセクションに存在する場合は<code>.export.*</code>疑似命令で対象を明示します。


### <code>.export</code>例

```
            .export         func2
            .export         func2, "nfunc2"    ;# func2 を nfunc2 として開示
            .export.func    func3
            .export.func    func3, "nfunc3"    ;# func3 を nfunc3 として開示

            .export         tab1
            .export         tab1, "ntab1"      ;# tab1 を ntab1 として開示
            .export.table   tab2
            .export.table   tab2, "ntab2"      ;# tab2 を ntab2 として開示

            .export         mem1
            .export         mem1, "nmem1"      ;# mem1 を nmem1 として開示
            .export.memory  mem2
            .export.memory  mem2, "nmem2"      ;# mem2 を nmem2 として開示

            .export         global2
            .export         global3, "gconst"  ;# global3 を gconst として開示
            .export.global  global5
            .export.global  global6, "gvar"    ;# global6 を gvar として開示
```


## Custom Section

<details><summary><code>.custom</code>疑似命令</summary><div>

```
            .custom         "name", <section-name>
            ;# 以下、カスタム セクション用データ
```

<code>.custom</code>疑似命令で、Custom Section を設定します。

カスタム セクションは各セクションの後に1つだけ追加できます。

&lt;section-name&gt; には

+ "custom", "customsec"
+ "type", "typesec"
+ "import", "importsec"
+ "function", "functionsec", "func"
+ "table", "tablesec"
+ "memory", "memsec", "mem"
+ "global", "globalsec"
+ "export", "exportsec"
+ "start", "startsec"
+ "element", "elementsec", "elem"
+ "code", "codesec"
+ "data", "datasec"
+ "datacount", "datacountsec"

を指定できます。指定なしの場合は "custom" と同じです。

</div></details>


## アセンブラ用疑似命令

アセンブラ用疑似命令は <code>@</code> で始まる識別子を使用します。ラベルは無視されます。

アセンブラ用疑似命令向けに真偽(<code>bool</code>)値が定義されています。

| 0 | 1 |
|:-:|:-:|
| false | true |
| no | yes |
| off | on |

注意：アセンブラ用疑似命令以外では未定義扱いです。


## <code>@option</code>疑似命令

<code>@option</code>疑似命令ではアセンブラの動作を指定します。


### オプション <code>include.depth</code>

<code>@include</code>疑似命令で入れ子の深さを設定します。値は<code>0〜100</code>の範囲に制限されます。

```
@option include.depth = <int>   ;# 既定=10
```


### オプション <code>comment.nest</code>

<code>/\* ... \*/</code> の入れ子対応を指定します。

```
@option comment.nest = <bool>   ;# 既定=false
```


### オプション <code>paren.multiline</code>

括弧<code>(),[],{}</code>の間の改行許可を指定します。

```
@option paren.multiline = <bool>   ;# 既定=false
```


### オプション <code>section.datacount</code>

.wasm ファイルへの Data Count Section 出力を制御します。

```
@option section.datacount = <bool>   ;# 既定=false
```


### オプション <code>type.unique</code>

.wasm のファイル サイズ縮小を目的としたオプションです。Type Section にある関数型の「引数型リストと返値型リスト」が一致するものを同一の<code>typeidx</code>とする場合は true を指定します。

```
@option type.unique = <bool>   ;# 既定=false
```


## <code>@set,@unset</code>疑似命令

<code>@set</code>疑似命令ではアセンブラ用変数(識別子)を設定します。

```
@set <識別子> = <値>
```

<code>@unset</code>疑似命令ではアセンブラ用変数(識別子)を削除します。

```
@unset <識別子>
```


## <code>@if,@elif,@else,@endif</code>疑似命令

<code>@if,@elif,@else,@endif</code>疑似命令でソースのアセンブル部分を制御できます。

```
@if 式1

;# 式1 が真のときにアセンブルされる
;# ...

@elif 式2

;# 式2 が真のときにアセンブルされる
;# ...

@else

;# 式1, 式2 ともに偽のときにアセンブルされる
;# ...

@endif
```

式で使用できる識別子は、アセンブラ用変数、<code>.alias</code>疑似命令のラベル名、オプション名になります。


## <code>@message</code>疑似命令

<code>@message</code>疑似命令では、アセンブリ時のアセンブラ変数などをメッセージとして出力することができます。

```
@message 文字列, 識別子, 数値, ...
```


## <code>@include</code>疑似命令

<code>@include</code>疑似命令では、その位置に指定ファイルの内容を挿入します。検索パス情報として <code>WASMGEN_INCLUDE</code> 環境変数を使用します。内容は<code>PATH</code>環境変数と同様です。

```
@include ファイル, exist
```

<code>exist</code>は省略可能で、<code>true</code>でファイルが開けない場合にエラーとなります。省略すると<code>true</code>です。


# 実行命令

詳細は [WebAssembly - Instructions](https://webassembly.github.io/spec/core/binary/instructions.html) を参照してください。


## 制御ブロック


### ブロックの開始

ブロックの開始は<code>block,loop,if</code>命令の3つ。

```
<ラベル>    block   <blocktype>
<ラベル>    loop    <blocktype>
<ラベル>    if      <blocktype>
```

<code>&lt;blocktype&gt;</code>は省略可能で<code>valtype</code>または<code>typeidx</code>ですが、<code>返値型リスト</code>または<code>{[引数型リスト],[返値型リスト]}</code>形式から<code>typeidx</code>を生成できます。

<code>ラベル</code>は制御ブロックの<code>labelidx</code>になります。

```
;# 記述パターン: <instr> は block, loop, if のどれか
            <instr>
            <instr>     <typeidx>
            <instr>     <valtype>
            <instr>     <valtype>, <valtype>, ...    ;# 返値型リストから<typeidx>を割り当てる
            <instr>     {[引数型リスト],[返値型リスト]}  ;# 関数型リストから<typeidx>を割り当てる
```


### ブロックの終了

ブロックの終了は<code>end</code>命令です。

```
<ラベル>    end
```

<code>ラベル</code>は開始側に記述したのと同じ効果がありますが、開始側に存在する場合は無視されます。<code>ラベル</code>の寿命は<code>end</code>命令で終了するので、同じラベル名を別のブロックで使用できます。


### 制御ブロックの例

```
n           .param      i32
p           .local      i32
            block
            local.get   n
            local.tee   p
            i32.eqz
            br_if       $exit
$cont:      loop
            local.get   p
            i32.const   1
            i32.sub
            local.tee   p
            br_if       $cont
            end
$exit:      end
```


## 比較命令の拡張

以下の命令を <code>&lt;cmp&gt;</code> とします。

| | <code>i32</code> | <code>i64</code> | <code>f32</code> | <code>f64</code> |
|:--|:--|:--|:--|:--|
| <code>eqz</code> | <code>i32.eqz</code> | <code>i64.eqz</code> | | |
| <code>eq</code> | <code>i32.eq</code> | <code>i64.eq</code> | <code>f32.eq</code> | <code>f64.eq</code> |
| <code>ne</code> | <code>i32.ne</code> | <code>i64.ne</code> | <code>f32.ne</code> | <code>f64.ne</code> |
| <code>lt</code> | | | <code>f32.lt</code> | <code>f64.lt</code> |
| <code>lt\_s</code> | <code>i32\.lt\_s</code> | <code>i64\.lt\_s</code> | | |
| <code>lt\_u</code> | <code>i32\.lt\_u</code> | <code>i64\.lt\_u</code> | | |
| <code>le</code> | | | <code>f32.le</code> | <code>f64.le</code> |
| <code>le\_s</code> | <code>i32\.le\_s</code> | <code>i64\.le\_s</code> | | |
| <code>le\_u</code> | <code>i32\.le\_u</code> | <code>i64\.le\_u</code> | | |

<code>&lt;cmp&gt;</code>にオペランドを記述すると分岐処理も追加します。


### オペランドに <code>&lt;labelidx&gt;</code> を記述

```
            <cmp>       <labelidx>
```

これは以下の2命令に展開されます。

```
            <cmp>
            br_if       <labelidx>
```


### オペランドに <code>return</code> を記述

```
            <cmp>       return
```

これは以下の4命令に展開されます。

```
            <cmp>
            if
              return
            end
```


## <code>v128.const</code>代替命令

以下の命令は<code>v128.const</code>の別名とし、オペランドは2番目以降を省略すると<code>0</code>として処理します。

| 命令 | 例 |
|:-:|:--|
| <code>v128.const</code> | <code>v128.const   0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15</code> |
| <code>i8x16.const</code> | <code>i8x16.const  0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15</code> |
| <code>i16x8.const</code> | <code>i16x8.const  0x0100,0x0302,0x0504,0x0706,0x0908,0x0b0a,0x0d0c,0x0f0e</code> |
| <code>i32x4.const</code> | <code>i32x4.const  0x03020100,0x07060504,0x0b0a0908,0x0f0e0d0c</code> |
| <code>i62x2.const</code> | <code>i64x2.const  0x0706050403020100,0x0f0e0d0c0b0a0908</code> |
| <code>f32x4.const</code> | <code>f32x4.const  1.0, 2.0, 3.0, 4.0</code> |
| <code>f62x2.const</code> | <code>f64x2.const  0.5, 0.25</code> |
| | |
| <code>v128.const</code> | <code>v128.const   0    ;# 2番目以降も 0 |


## メモリ命令

オペランドが<code>memarg</code>の命令を<code>&lt;memins&gt;</code>とすると、以下の記述パターンになります。

```
            <memins>                    ;# offset=0, align=<memins既定>
            <memins>    offset          ;# align=<memins既定>
            <memins>    offset, align
```

### <code>&lt;memins&gt;</code>と既定<code>align</code>

| 既定 align | &lt;memins&gt; |
|:-:|:--|
| 1 | <code>i32.load8\_s i32.load8\_u</code><br/><code>i64.load8\_s i64.load8\_u</code><br/><code>i32.store8 i64.store8</code><br/><code>v128.load8\_splat</code><br/><code>v128.load8\_lane</code><br/><code>v128.store8\_lane</code> |
| 2 | <code>i32.load16\_s i32.load16\_u</code><br/><code>i64.load16\_s i64.load16\_u</code><br/><code>i32.store16 i64.store16</code><br/><code>v128.load16\_splat</code><br/><code>v128.load16\_lane</code><br/><code>v128.store16\_lane</code> |
| 4 | <code>i32.load f32.load</code><br/><code>i64.load32\_s i64.load32\_u</code><br/><code>i32.store f32.store i64.store32</code><br/><code>v128.load32\_splat</code><br/><code>v128.load32\_lane v128.load32_zero</code><br/><code>v128.store32\_lane</code> |
| 8 | <code>i64.load f64.load i64.store f64.store</code><br/><code>v128.load8x8\_s v128.load8x8\_u</code><br/><code>v128.load16x4\_s v128.load16x4\_u</code><br/><code>v128.load32x2\_s v128.load32x2\_u</code><br/><code>v128.load64\_splat</code><br/><code>v128.load64\_lane v128.load64\_zero</code><br/><code>v128.store64\_lane</code> |
| 16 | <code>v128.load v128.store</code> |


# 命令表


### [仕様書](https://webassembly.github.io/spec/core/exec/instructions.html)にある配列の略記

| インデックス<code>𝑥</code> | 略記 | 仕様表記 |
|:--|:--|:--|
| <code>funcidx</code> | <code>FUNC[𝑥]</code> | <code>S.tables[F.module.funcaddrs[𝑥]]</code> |
| <code>tableidx</code> | <code>TAB[𝑥]</code> | <code>S.tables[F.module.tableaddrs[𝑥]].elem</code> |
| <code>memidx</code> | <code>MEM[𝑥]</code> | <code>S.mems[F.module.elemaddrs[𝑥]].elem</code> |
| <code>dataidx</code> |<code>DATA[𝑥]</code> | <code>S.datas[F.module.dataaddrs[𝑥]].data</code> |

また、<code>MEM0 = MEM[0]</code> とします。


### 内容・備考などでの機能表現

処理表現を JavaScript 風にすると以下のようになります。

| 機能 | 出力 | 備考 |
|:-:|:--|:--|
| <code>abs(x)</code> | <code>x &lt; 0 ? -x : x</code> | 絶対値 |
| <code>sqrt(x)</code> | <code>√x</code> | 平方根 |
| <code>ceil(x)</code> | <code>x</code>以上の整数 | 天井関数（切り上げ） |
| <code>nearest(x)</code> | <code>x</code>を四捨五入した整数 | 丸め関数（四捨五入） |
| <code>floor(x)</code> | <code>x</code>以下の整数 | 床関数（切り捨て） |
| <code>trunc(x)</code> | <code>x</code>の小数部を切り捨て | ゼロ方向丸め関数 |
| | | |
| <code>minU(x)</code> | <code>0</code> | 符号なし<code>x</code>ビットの最小値<code>0</code> |
| <code>maxU(x)</code> | <code>(1 &lt;&lt; b) - 1</code> | 符号なし<code>x</code>ビットの最大値<code>2<sup>x</sup>-1</code> |
| <code>minS(x)</code> | <code>- (1 &lt;&lt; (b - 1))</code> | 符号あり<code>x</code>ビットの最小値<code>-2<sup>x-1</sup></code> |
| <code>maxS(x)</code> | <code>+ (1 &lt;&lt; (b - 1)) - 1</code> | 符号あり<code>x</code>ビットの最大値<code>+2<sup>x-1</sup>-1</code> |
| | | |
| <code>sign(x)</code> | <code>x &lt; 0 ? 1 : 0</code> | 符号ビット |
| <code>signN(x,N)</code> | <code>(x &gt;&gt; (N - 1)) &amp; 1</code> | <code>N</code>ビット データの符号 |
| <code>sign8(x)</code> | <code>signN(8)</code> | 8ビット データの符号 |
| <code>sign16(x)</code> | <code>signN(16)</code> | 16ビット データの符号 |
| <code>sign32(x)</code> | <code>signN(32)</code> | 32ビット データの符号 |
| <code>sign64(x)</code> | <code>signN(64)</code> | 64ビット データの符号 |
| | | |
| <code>ext8u(x)</code> | <code>x &amp; maxU(8)</code> | 下位8ビットが有効なゼロ拡張 |
| <code>ext16u(x)</code> | <code>x &amp; maxU(16)</code> | 下位16ビットが有効なゼロ拡張 |
| <code>ext32u(x)</code> | <code>x &amp; maxU(32)</code> | 下位32ビットが有効なゼロ拡張 |
| | | |
| <code>extM(x,b)</code> | <code>signN(x,b) * ~maxU(b)</code> | 符号拡張用マスク値 |
| <code>ext8s(x)</code> | <code>ext8u(x) \| extM(x, 8)</code> | ビット7を符号として上位ビットに拡張 |
| <code>ext16s(x)</code> | <code>ext16u(x) \| extM(x, 16)</code> | ビット15を符号として上位ビットに拡張 |
| <code>ext32s(x)</code> | <code>ext32u(x) \| extM(x, 32)</code> | ビット31を符号として上位ビットに拡張 |
| | | |
| <code>min(x,y)</code> | <code>x &lt; y ? x : y</code> | <code>x</code>と<code>y</code>の小さい方 |
| <code>pmin(x,y)</code> | 同上 | <code>min(x,y)</code> の無限・非数の扱い違い版 |
| <code>max(x,y)</code> | <code>x &gt; y ? x : y</code> | <code>x</code>と<code>y</code>の大きい方 |
| <code>pmin(x,y)</code> | 同上 | <code>max(x,y)</code> の無限・非数の扱い違い版 |
| <code>clamp(s,x,e)</code> | <code>max(s, min(x, e))</code> | <code>x</code>を<code>s</code>から<code>e</code>の範囲内に留める |
| | | |
| <code>satS(x,b)</code> | <code>clamp(minS(b), x, maxS(b))</code> | <code>x</code>を<code>-2<sup>b-1</sup></code>から<code>2<sup>b-1</sup>-1</code>の範囲内に留める |
| <code>sat8s(x)</code> | <code>satS(x, 8)</code> | <code>x</code>を<code>-2<sup>7</sup></code>から<code>2<sup>7</sup>-1</code>の範囲内に留める |
| <code>sat16s(x)</code> | <code>satS(x, 16)</code> | <code>x</code>を<code>-2<sup>15</sup></code>から<code>2<sup>15</sup>-1</code>の範囲内に留める |
| <code>sat32s(x)</code> | <code>satS(x, 32)</code> | <code>x</code>を<code>-2<sup>31</sup></code>から<code>2<sup>31</sup>-1</code>の範囲内に留める |
| <code>sat64s(x)</code> | <code>satS(x, 64)</code> | <code>x</code>を<code>-2<sup>63</sup></code>から<code>2<sup>63</sup>-1</code>の範囲内に留める |
| | | |
| <code>satU(x,b)</code> | <code>clamp(0, x, (1&lt;&lt;b)-1)</code> | <code>x</code>を<code>0</code>から<code>2<sup>b</sup>-1</code>の範囲内に留める |
| <code>sat8u(x)</code> | <code>satU(x, 8)</code> | <code>x</code>を<code>0</code>から<code>2<sup>8</sup>-1</code>の範囲内に留める |
| <code>sat16u(x)</code> | <code>satU(x, 16)</code> | <code>x</code>を<code>0</code>から<code>2<sup>16</sup>-1</code>の範囲内に留める |
| <code>sat32u(x)</code> | <code>satU(x, 32)</code> | <code>x</code>を<code>0</code>から<code>2<sup>32</sup>-1</code>の範囲内に留める |
| <code>sat64u(x)</code> | <code>satU(x, 64)</code> | <code>x</code>を<code>0</code>から<code>2<sup>64</sup>-1</code>の範囲内に留める |
| | | |
| <code>copysign(x,y)</code> | <code>y / abs(y) * abs(x)</code> | <code>x</code>の符号を<code>y</code>の符号にする |
| | | |
| <code>clzN(x,b)</code> | 最上位ビットから続く<code>0</code>の数 | 最小は<code>0</code>,最大で<code>b</code> |
| <code>clz32(x)</code> | <code>clzN(x, 32)</code> | <code>clz</code> の32ビット版 |
| <code>clz64(x)</code> | <code>clzN(x, 64)</code> | <code>clz</code> の64ビット版 |
| | | |
| <code>ctzN(x,b)</code> | 最下位ビットから続く<code>0</code>の数 | 最小は<code>0</code>,最大で<code>b</code> |
| <code>ctz32(x)</code> | <code>ctzN(x, 32)</code> | <code>ctz</code> の32ビット版 |
| <code>ctz64(x)</code> | <code>ctzN(x, 64)</code> | <code>ctz</code> の64ビット版 |
| | | |
| <code>popcnt(x)</code> | <code>x</code>の2進数表現で<code>1</code>の数 | |
| | | |
| <code>rotlN(x,n,b)</code> | <code>(x&lt;&lt;n)\|((x&amp;maxU(b))&gt;&gt;&gt;(b-n))</code> | 左ローテート |
| <code>rotl32(x,n)</code> | <code>rotlN(x, n, 32)</code> | 32ビット左ローテート |
| <code>rotl64(x,n)</code> | <code>rotlN(x, n, 64)</code> | 64ビット左ローテート |
| <code>rotrN(x,n,b)</code> | <code>((x&amp;maxU(b))&gt;&gt;&gt;n)\|(x&lt;&lt;(b-n))</code> | 右ローテート |
| <code>rotr32(x,n)</code> | <code>rotrN(x, n, 32)</code> | 32ビット右ローテート |
| <code>rotr64(x,n)</code> | <code>rotrN(x, n, 64)</code> | 64ビット右ローテート |
| | | |
| <code>avgr(x, y)</code>  | <code>(x + y + 1) &gt;&gt; 1</code> | 加算平均(四捨五入) |
| <code>i15mul(x, y)</code>  | <code>(x * y + (1<<14)) &gt;&gt; 15</code> | 15ビット固定小数点乗算 |


## 命令表の各列

| 列 | 内容 |
|:-:|:--|
| 引数 | 実行に必要な引数の数で、最初の引数の記号は<code><var>p1</var></code>で<code><var>p2</var>,<var>p3</var>...</code>と続く |
| 返値 | 実行結果で、記号<code><var>r</var></code>とする |
| 命令 | コード(同一セル内に別名も列挙) |
| OP | 命令のオペランド |
| 備考 | 主に動作内容 |


## 定数

| 引数 | 返値 | 命令 | OP | 備考 |
|:-:|:-:|:--|:--|:--|
| <code>0</code><br/>&nbsp;<br/>&nbsp; | <code>i32</code><br/>&nbsp;<br/>&nbsp; | <code>i32\.const</code><br/><code>s32\.const</code><br/><code>u32\.const</code> | <code>i32</code><br/>&nbsp;<br/>&nbsp; | <code><var>r</var><sub>i32</sub> ← <var>i32</var></code> <br/>&nbsp;<br/>&nbsp; |
| <code>0</code><br/>&nbsp;<br/>&nbsp; | <code>i64</code><br/>&nbsp;<br/>&nbsp; | <code>i64\.const</code><br/><code>s64\.const</code><br/><code>u64\.const</code> | <code>i64</code><br/>&nbsp;<br/>&nbsp; | <code><var>r</var><sub>i64</sub> ← <var>i64</var></code> <br/>&nbsp;<br/>&nbsp; |
| <code>0</code> | <code>f32</code> | <code>f32\.const</code> | <code>f32</code> | <code><var>r</var><sub>f32</sub> ← <var>f32</var></code> |
| <code>0</code> | <code>f64</code> | <code>f64\.const</code> | <code>f64</code> | <code><var>r</var><sub>f64</sub> ← <var>f64</var></code> |
| <code>0</code><br/>&nbsp;<br/>&nbsp; | <code>v128</code><br/>&nbsp;<br/>&nbsp; | <code>i8x16\.const</code><br/><code>s8x16\.const</code><br/><code>u8x16\.const</code> | <code>i8<sub>1</sub>,i8<sub>2</sub>,...,i8<sub>16</sub></code><br/>&nbsp;<br/>&nbsp; | <code><var>r</var><sub>v128</sub> ← <var>v128</var></code> <br/>&nbsp;<br/>&nbsp; |
| <code>0</code><br/>&nbsp;<br/>&nbsp; | <code>v128</code><br/>&nbsp;<br/>&nbsp; | <code>i16x8\.const</code><br/><code>s16x8\.const</code><br/><code>u16x8\.const</code> | <code>i16<sub>1</sub>,i16<sub>2</sub>,...,i16<sub>8</sub></code><br/>&nbsp;<br/>&nbsp; | <code><var>r</var><sub>v128</sub> ← <var>v128</var></code> <br/>&nbsp;<br/>&nbsp; |
| <code>0</code><br/>&nbsp;<br/>&nbsp; | <code>v128</code><br/>&nbsp;<br/>&nbsp; | <code>i32x4\.const</code><br/><code>s32x4\.const</code><br/><code>u32x4\.const</code> | <code>i32<sub>1</sub>,i32<sub>2</sub>,i32<sub>3</sub>,i32<sub>4</sub></code><br/>&nbsp;<br/>&nbsp; | <code><var>r</var><sub>v128</sub> ← <var>v128</var></code> <br/>&nbsp;<br/>&nbsp; |
| <code>0</code><br/>&nbsp;<br/>&nbsp; | <code>v128</code><br/>&nbsp;<br/>&nbsp; | <code>i64x2\.const</code><br/><code>s64x2\.const</code><br/><code>u64x2\.const</code> | <code>i64<sub>1</sub>,i64<sub>2</sub></code><br/>&nbsp;<br/>&nbsp; | <code><var>r</var><sub>v128</sub> ← <var>v128</var></code> <br/>&nbsp;<br/>&nbsp; |
| <code>0</code> | <code>v128</code> | <code>f32x4\.const</code> | <code>f32<sub>1</sub>,f32<sub>2</sub>,f32<sub>3</sub>,f32<sub>4</sub></code> | <code><var>r</var><sub>v128</sub> ← <var>v128</var></code> |
| <code>0</code> | <code>v128</code> | <code>f64x2\.const</code> | <code>f64<sub>1</sub>,f64<sub>2</sub></code> | <code><var>r</var><sub>v128</sub> ← <var>v128</var></code> |
| <code>0</code> | <code>v128</code> | <code>v128\.const</code> | <code>i8<sub>1</sub>,i8<sub>2</sub>,...,i8<sub>16</sub></code> | <code><var>r</var><sub>v128</sub> ← <var>v128</var></code> |


## ローカル変数

| 引数 | 返値 | 命令 | OP<code>𝑥</code> | 備考 |
|:-:|:-:|:--|:--|:--|
| <code>1</code> | | <code>local\.get</code> | <code>localidx</code> | <code><var>r</var> ← locals[𝑥]</code> |
| <code>0</code> | <code>any</code> | <code>local\.set</code> | <code>localidx</code> | <code>locals[𝑥] ← <var>p1</var></code> |
| <code>1</code> | <code>any</code> | <code>local\.tee</code> | <code>localidx</code> | <code><var>r</var> ← <var>p1</var> ; locals[𝑥] ← <var>p1</var></code>  |


## グローバル変数

| 引数 | 返値 | 命令 | OP<code>𝑥</code> | 備考 |
|:-:|:-:|:--|:--|:--|
| <code>0</code> | <code>any</code> | <code>global\.get</code> | <code>globalidx</code> | <code><var>r</var> ← globals[𝑥]</code> |
| <code>1</code> | | <code>global\.set</code> | <code>globalidx</code> | <code>globals[𝑥] ← <var>p1</var></code> |


## テーブル・アクセス

| 引数 | 返値 | 命令 | OP<code>𝑥</code> | 備考 |
|:-:|:-:|:--|:--|:--|
| <code>1</code> | <code>func</code> | <code>table\.get</code> | <code>tableidx</code> | <code><var>r</var><sub>func</sub> ← TAB[𝑥]\[<var>p1</var><sub>i32</sub>]</code> |
| <code>2</code> | | <code>table\.set</code> | <code>tableidx</code> | <code>TAB[𝑥]\[<var>p1</var><sub>i32</sub>] ← <var>p2</var></code> |


## メモリ・アクセス

| 引数 | 返値 | 命令 | OP | 備考 (<code>𝑚 = オフセット</code>) |
|:-:|:-:|:--|:--|:--|
| <code>1</code><br/>&nbsp;<br/>&nbsp; | <code>i32</code><br/>&nbsp;<br/>&nbsp; | <code>i32\.load</code><br/><code>s32\.load</code><br/><code>u32\.load</code> | <code>memarg</code><br/>&nbsp;<br/>&nbsp; | <code><var>r</var><sub>i32</sub> ← MEM0[𝑚 + <var>p1</var><sub>i32</sub>]<sub>i32</sub></code> <br/>&nbsp;<br/>&nbsp; |
| <code>1</code><br/>&nbsp;<br/>&nbsp; | <code>i64</code><br/>&nbsp;<br/>&nbsp; | <code>i64\.load</code><br/><code>s64\.load</code><br/><code>u64\.load</code> | <code>memarg</code><br/>&nbsp;<br/>&nbsp; | <code><var>r</var><sub>i64</sub> ← MEM0[𝑚 + <var>p1</var><sub>i32</sub>]<sub>i64</sub></code> <br/>&nbsp;<br/>&nbsp; |
| <code>1</code> | <code>f32</code> | <code>f32\.load</code> | <code>memarg</code> | <code><var>r</var><sub>f32</sub> ← MEM0[𝑚 + <var>p1</var><sub>i32</sub>]<sub>f32</sub></code> |
| <code>1</code> | <code>f64</code> | <code>f64\.load</code> | <code>memarg</code> | <code><var>r</var><sub>f64</sub> ← MEM0[𝑚 + <var>p1</var><sub>i32</sub>]<sub>f64</sub></code> |
| | | | | |
| <code>1</code><br/>&nbsp; | <code>i32</code><br/>&nbsp; | <code>i32\.load8\_s</code><br/><code>s32\.load8</code> | <code>memarg</code><br/>&nbsp; | <code><var>r</var><sub>i32</sub> ← ext8s(MEM0[𝑚 + <var>p1</var><sub>i32</sub>]<sub>i8</sub>)</code> <br/>&nbsp; |
| <code>1</code><br/>&nbsp; | <code>i32</code><br/>&nbsp; | <code>i32\.load16\_s</code><br/><code>s32\.load16</code> | <code>memarg</code><br/>&nbsp; | <code><var>r</var><sub>i32</sub> ← ext16s(MEM0[𝑚 + <var>p1</var><sub>i32</sub>]<sub>i16</sub>)</code> <br/>&nbsp; |
| <code>1</code><br/>&nbsp; | <code>i64</code><br/>&nbsp; | <code>i64\.load8\_s</code><br/><code>s64\.load8</code> | <code>memarg</code><br/>&nbsp; | <code><var>r</var><sub>i64</sub> ← ext8s(MEM0[𝑚 + <var>p1</var><sub>i32</sub>]<sub>i8</sub>)</code> <br/>&nbsp; |
| <code>1</code><br/>&nbsp; | <code>i64</code><br/>&nbsp; | <code>i64\.load16\_s</code><br/><code>s64\.load16</code> | <code>memarg</code><br/>&nbsp; | <code><var>r</var><sub>i64</sub> ← ext16s(MEM0[𝑚 + <var>p1</var><sub>i32</sub>]<sub>i16</sub>)</code> <br/>&nbsp; |
| <code>1</code><br/>&nbsp; | <code>i64</code><br/>&nbsp; | <code>i64\.load32\_s</code><br/><code>s64\.load32</code> | <code>memarg</code><br/>&nbsp; | <code><var>r</var><sub>i64</sub> ← ext32s(MEM0[𝑚 + <var>p1</var><sub>i32</sub>]<sub>i32</sub>)</code> <br/>&nbsp; |
| | | | | |
| <code>1</code><br/>&nbsp; | <code>i32</code><br/>&nbsp; | <code>i32\.load8\_u</code><br/><code>u32\.load8</code> | <code>memarg</code><br/>&nbsp; | <code><var>r</var><sub>i32</sub> ← ext8u(MEM0[𝑚 + <var>p1</var><sub>i32</sub>]<sub>i8</sub>)</code> <br/>&nbsp; |
| <code>1</code><br/>&nbsp; | <code>i32</code><br/>&nbsp; | <code>i32\.load16\_u</code><br/><code>u32\.load16</code> | <code>memarg</code><br/>&nbsp; | <code><var>r</var><sub>i32</sub> ← ext16u(MEM0[𝑚 + <var>p1</var><sub>i32</sub>]<sub>i16</sub>)</code> <br/>&nbsp; |
| <code>1</code><br/>&nbsp; | <code>i64</code><br/>&nbsp; | <code>i64\.load8\_u</code><br/><code>u64\.load8</code> | <code>memarg</code><br/>&nbsp; | <code><var>r</var><sub>i64</sub> ← ext8u(MEM0[𝑚 + <var>p1</var><sub>i32</sub>]<sub>i8</sub>)</code> <br/>&nbsp; |
| <code>1</code><br/>&nbsp; | <code>i64</code><br/>&nbsp; | <code>i64\.load16\_u</code><br/><code>u64\.load16</code> | <code>memarg</code><br/>&nbsp; | <code><var>r</var><sub>i64</sub> ← ext16u(MEM0[𝑚 + <var>p1</var><sub>i32</sub>]<sub>i16</sub>)</code> <br/>&nbsp; |
| <code>1</code><br/>&nbsp; | <code>i64</code><br/>&nbsp; | <code>i64\.load32\_u</code><br/><code>u64\.load32</code> | <code>memarg</code><br/>&nbsp; | <code><var>r</var><sub>i64</sub> ← ext32u(MEM0[𝑚 + <var>p1</var><sub>i32</sub>]<sub>i32</sub>)</code> <br/>&nbsp; |
| | | | | |
| <code>2</code><br/>&nbsp;<br/>&nbsp; | | <code>i32\.store</code><br/><code>s32\.store</code><br/><code>u32\.store</code> | <code>memarg</code><br/>&nbsp;<br/>&nbsp; | <code>MEM0[𝑚 + <var>p1</var><sub>i32</sub>]<sub>i32</sub> ← <var>p2</var><sub>i32</sub></code> <br/>&nbsp;<br/>&nbsp; |
| <code>2</code><br/>&nbsp;<br/>&nbsp; | | <code>i64\.store</code><br/><code>s64\.store</code><br/><code>u64\.store</code> | <code>memarg</code><br/>&nbsp;<br/>&nbsp; | <code>MEM0[𝑚 + <var>p1</var><sub>i32</sub>]<sub>i64</sub> ← <var>p2</var><sub>i64</sub></code> <br/>&nbsp;<br/>&nbsp; |
| <code>2</code> | | <code>f32\.store</code> | <code>memarg</code> | <code>MEM0[𝑚 + <var>p1</var><sub>i32</sub>]<sub>f32</sub> ← <var>p2</var><sub>f32</sub></code> |
| <code>2</code> | | <code>f64\.store</code> | <code>memarg</code> | <code>MEM0[𝑚 + <var>p1</var><sub>i32</sub>]<sub>f64</sub> ← <var>p2</var><sub>f64</sub></code> |
| | | | | |
| <code>2</code><br/>&nbsp;<br/>&nbsp; | | <code>i32\.store8</code><br/><code>s32\.store8</code><br/><code>u32\.store8</code> | <code>memarg</code><br/>&nbsp;<br/>&nbsp; | <code>MEM0[𝑚 + <var>p1</var><sub>i32</sub>]<sub>i8</sub> ← <var>p2</var><sub>i32</sub></code> <br/>&nbsp;<br/>&nbsp; |
| <code>2</code><br/>&nbsp;<br/>&nbsp; | | <code>i32\.store16</code><br/><code>s32\.store16</code><br/><code>u32\.store16</code> | <code>memarg</code><br/>&nbsp;<br/>&nbsp; | <code>MEM0[𝑚 + <var>p1</var><sub>i32</sub>]<sub>i16</sub> ← <var>p2</var><sub>i32</sub></code> <br/>&nbsp;<br/>&nbsp; |
| <code>2</code><br/>&nbsp;<br/>&nbsp; | | <code>i64\.store8</code><br/><code>s64\.store8</code><br/><code>u64\.store8</code> | <code>memarg</code><br/>&nbsp;<br/>&nbsp; | <code>MEM0[𝑚 + <var>p1</var><sub>i32</sub>]<sub>i8</sub> ← <var>p2</var><sub>i64</sub></code> <br/>&nbsp;<br/>&nbsp; |
| <code>2</code><br/>&nbsp;<br/>&nbsp; | | <code>i64\.store16</code><br/><code>s64\.store16</code><br/><code>u64\.store16</code> | <code>memarg</code><br/>&nbsp;<br/>&nbsp; | <code>MEM0[𝑚 + <var>p1</var><sub>i32</sub>]<sub>i16</sub> ← <var>p2</var><sub>i64</sub></code> <br/>&nbsp;<br/>&nbsp; |
| <code>2</code><br/>&nbsp;<br/>&nbsp; | | <code>i64\.store32</code><br/><code>s64\.store32</code><br/><code>u64\.store32</code> | <code>memarg</code><br/>&nbsp;<br/>&nbsp; | <code>MEM0[𝑚 + <var>p1</var><sub>i32</sub>]<sub>i32</sub> ← <var>p2</var><sub>i64</sub></code> <br/>&nbsp;<br/>&nbsp; |

| 引数 | 返値 | 命令 | OP | 備考 (<code>𝑚 = オフセット</code>) |
|:-:|:-:|:--|:--|:--|
| <code>1</code><br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp; | <code>v128</code><br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp; | <code>v128\.load</code><br/><code>i8x16\.load</code><br/><code>s8x16\.load</code><br/><code>u8x16\.load</code><br/><code>i16x8\.load</code><br/><code>s16x8\.load</code><br/><code>u16x8\.load</code><br/><code>i32x4\.load</code><br/><code>s32x4\.load</code><br/><code>u32x4\.load</code><br/><code>i64x2\.load</code><br/><code>s64x2\.load</code><br/><code>u64x2\.load</code><br/><code>f32x4\.load</code><br/><code>f64x2\.load</code> | <code>memarg</code><br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp; | <code><var>r</var><sub>v128</sub> ← MEM0[𝑚 + <var>p1</var><sub>i32</sub>]<sub>v128</sub></code> <br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp; |
| <code>2</code><br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp; | | <code>v128\.store</code><br/><code>i8x16\.store</code><br/><code>s8x16\.store</code><br/><code>u8x16\.store</code><br/><code>i16x8\.store</code><br/><code>s16x8\.store</code><br/><code>u16x8\.store</code><br/><code>i32x4\.store</code><br/><code>s32x4\.store</code><br/><code>u32x4\.store</code><br/><code>i64x2\.store</code><br/><code>s64x2\.store</code><br/><code>u64x2\.store</code><br/><code>f32x4\.store</code><br/><code>f64x2\.store</code> | <code>memarg</code><br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp; | <code>MEM0[𝑚 + <var>p1</var><sub>i32</sub>]<sub>v128</sub> ← <var>p2</var><sub>v128</sub></code> <br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp; |
| | | | | |
| <code>1</code><br/>&nbsp; | <code>v128</code><br/>&nbsp; | <code>v128\.load8x8\_s</code><br/><code>s16x8\.load8x8</code> | <code>memarg</code><br/>&nbsp; | <code><var>r</var><sub>i16</sub>[𝑛] ← ext8s(MEM0[𝑚 + <var>p1</var><sub>i32</sub> + 𝑛]<sub>i8</sub>)</code> <br/>&nbsp; |
| <code>1</code><br/>&nbsp; | <code>v128</code><br/>&nbsp; | <code>v128\.load16x4\_s</code><br/><code>s32x4\.load16x4</code> | <code>memarg</code><br/>&nbsp; | <code><var>r</var><sub>i32</sub>[𝑛] ← ext16s(MEM0[𝑚 + <var>p1</var><sub>i32</sub> + 2𝑛]<sub>i16</sub>)</code> <br/>&nbsp; |
| <code>1</code><br/>&nbsp; | <code>v128</code><br/>&nbsp; | <code>v128\.load32x2\_s</code><br/><code>s64x2\.load32x2</code> | <code>memarg</code><br/>&nbsp; | <code><var>r</var><sub>i64</sub>[𝑛] ← ext32s(MEM0[𝑚 + <var>p1</var><sub>i32</sub> + 4𝑛]<sub>i32</sub>)</code> <br/>&nbsp; |
| | | | | |
| <code>1</code><br/>&nbsp; | <code>v128</code><br/>&nbsp; | <code>v128\.load8x8\_u</code><br/><code>u16x8\.load8x8</code> | <code>memarg</code><br/>&nbsp; | <code><var>r</var><sub>i16</sub>[𝑛] ← ext8u(MEM0[𝑚 + <var>p1</var><sub>i32</sub> + 𝑛]<sub>i8</sub>)</code> <br/>&nbsp; |
| <code>1</code><br/>&nbsp; | <code>v128</code><br/>&nbsp; | <code>v128\.load16x4\_u</code><br/><code>u32x4\.load16x4</code> | <code>memarg</code><br/>&nbsp; | <code><var>r</var><sub>i32</sub>[𝑛] ← ext16u(MEM0[𝑚 + <var>p1</var><sub>i32</sub> + 2𝑛]<sub>i16</sub>)</code> <br/>&nbsp; |
| <code>1</code><br/>&nbsp; | <code>v128</code><br/>&nbsp; | <code>v128\.load32x2\_u</code><br/><code>u64x2\.load32x2</code> | <code>memarg</code><br/>&nbsp; | <code><var>r</var><sub>i64</sub>[𝑛] ← ext32u(MEM0[𝑚 + <var>p1</var><sub>i32</sub> + 4𝑛]<sub>i32</sub>)</code> <br/>&nbsp; |
| <code>1</code><br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp; | <code>v128</code><br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp; | <code>v128\.load32\_zero</code><br/><code>i32x4.load\_zero</code><br/><code>s32x4.load\_zero</code><br/><code>u32x4.load\_zero</code><br/><code>f32x4.load\_zero</code> | <code>memarg</code><br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp; | <code><var>r</var><sub>v128</sub> ← ext32u(MEM0[𝑚 + <var>p1</var><sub>i32</sub>]<sub>i32</sub>)</code>  <br/> 上位96ビットはゼロ <br/>&nbsp;<br/>&nbsp;<br/>&nbsp; |
| <code>1</code><br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp; | <code>v128</code><br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp; | <code>v128\.load64\_zero</code><br/><code>i64x2\.load\_zero</code><br/><code>s64x2\.load\_zero</code><br/><code>u64x2\.load\_zero</code><br/><code>f64x2\.load\_zero</code> | <code>memarg</code><br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp; | <code><var>r</var><sub>v128</sub> ← ext64u(MEM0[𝑚 + <var>p1</var><sub>i32</sub>]<sub>i64</sub>)</code> <br/> 上位64ビットはゼロ <br/>&nbsp;<br/>&nbsp;<br/>&nbsp; |
| | | | | |
| <code>1</code><br/>&nbsp;<br/>&nbsp;<br/>&nbsp; | <code>v128</code><br/>&nbsp;<br/>&nbsp;<br/>&nbsp; | <code>v128\.load8\_splat</code><br/><code>i8x16\.load\_splat</code><br/><code>s8x16\.load\_splat</code><br/><code>u8x16\.load\_splat</code> | <code>memarg</code><br/>&nbsp;<br/>&nbsp;<br/>&nbsp; | <code><var>r</var><sub>i8x16</sub>[0..15] ← MEM0[𝑚 + <var>p1</var><sub>i32</sub>]<sub>i8</sub></code> <br/>&nbsp;<br/>&nbsp;<br/>&nbsp; |
| <code>1</code><br/>&nbsp;<br/>&nbsp;<br/>&nbsp; | <code>v128</code><br/>&nbsp;<br/>&nbsp;<br/>&nbsp; | <code>v128\.load16\_splat</code><br/><code>i16x8\.load\_splat</code><br/><code>s16x8\.load\_splat</code><br/><code>u16x8\.load\_splat</code> | <code>memarg</code><br/>&nbsp;<br/>&nbsp;<br/>&nbsp; | <code><var>r</var><sub>i16x8</sub>[0..7] ← MEM0[𝑚 + <var>p1</var><sub>i32</sub>]<sub>i16</sub></code> <br/>&nbsp;<br/>&nbsp;<br/>&nbsp; |
| <code>1</code><br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp; | <code>v128</code><br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp; | <code>v128\.load32\_splat</code><br/><code>i32x4\.load\_splat</code><br/><code>s32x4\.load\_splat</code><br/><code>u32x4\.load\_splat</code><br/><code>f32x4\.load\_splat</code> | <code>memarg</code><br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp; | <code><var>r</var><sub>i32x4</sub>[0..3] ← MEM0[𝑚 + <var>p1</var><sub>i32</sub>]<sub>i32</sub></code> <br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp; |
| <code>1</code><br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp; | <code>v128</code><br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp; | <code>v128\.load64\_splat</code><br/><code>i64x2\.load\_splat</code><br/><code>s64x2\.load\_splat</code><br/><code>u64x2\.load\_splat</code><br/><code>f64x2\.load\_splat</code> | <code>memarg</code><br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp; | <code><var>r</var><sub>i64x2</sub>[0..1] ← MEM0[𝑚 + <var>p1</var><sub>i32</sub>]<sub>i64</sub></code> <br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp; |
| | | | | |
| <code>2</code><br/>&nbsp;<br/>&nbsp;<br/>&nbsp; | <code>v128</code><br/>&nbsp;<br/>&nbsp;<br/>&nbsp; | <code>v128\.load8\_lane</code><br/><code>i8x16\.load\_lane</code><br/><code>s8x16\.load\_lane</code><br/><code>u8x16\.load\_lane</code> | <code>lane,memarg</code><br/>&nbsp;<br/>&nbsp;<br/>&nbsp; | <code><var>r</var><sub>i8x16</sub> ← p2<sub>v128</sub></code><br/><code><var>r</var><sub>i8x16</sub>[𝑛] ← MEM0[𝑚 + <var>p1</var><sub>i32</sub>]<sub>i8</sub></code> <br/>&nbsp;<br/>&nbsp; |
| <code>2</code><br/>&nbsp;<br/>&nbsp;<br/>&nbsp; | <code>v128</code><br/>&nbsp;<br/>&nbsp;<br/>&nbsp; | <code>v128\.load16\_lane</code><br/><code>i16x8\.load\_lane</code><br/><code>s16x8\.load\_lane</code><br/><code>u16x8\.load\_lane</code> | <code>lane,memarg</code><br/>&nbsp;<br/>&nbsp;<br/>&nbsp; | <code><var>r</var><sub>i16x8</sub> ← p2<sub>v128</sub></code><br/><code><var>r</var><sub>i16x8</sub>[𝑛] ← MEM0[𝑚 + <var>p1</var><sub>i32</sub>]<sub>i16</sub></code> <br/>&nbsp;<br/>&nbsp; |
| <code>2</code><br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp; | <code>v128</code><br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp; | <code>v128\.load32\_lane</code><br/><code>i32x4\.load\_lane</code><br/><code>s32x4\.load\_lane</code><br/><code>u32x4\.load\_lane</code><br/><code>f32x4\.load\_lane</code> | <code>lane,memarg</code><br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp; | <code><var>r</var><sub>i32x4</sub> ← p2<sub>v128</sub></code><br/><code><var>r</var><sub>i32x4</sub>[𝑛] ← MEM0[𝑚 + <var>p1</var><sub>i32</sub>]<sub>i32</sub></code><br/>&nbsp;<br/>&nbsp;<br/>&nbsp; |
| <code>2</code><br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp; | <code>v128</code><br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp; | <code>v128\.load64\_lane</code><br/><code>i64x2\.load\_lane</code><br/><code>s64x2\.load\_lane</code><br/><code>u64x2\.load\_lane</code><br/><code>f64x2\.load\_lane</code> | <code>lane,memarg</code><br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp; | <code><var>r</var><sub>i64x2</sub> ← p2<sub>v128</sub></code><br/><code><var>r</var><sub>i64x2</sub>[𝑛] ← MEM0[𝑚 + <var>p1</var><sub>i32</sub>]<sub>i64</sub></code> <br/>&nbsp;<br/>&nbsp;<br/>&nbsp; |
| | | | | |
| <code>2</code><br/>&nbsp;<br/>&nbsp;<br/>&nbsp; | | <code>v128\.store8\_lane</code><br/><code>i8x16\.store\_lane</code><br/><code>s8x16\.store\_lane</code><br/><code>u8x16\.store\_lane</code> | <code>lane,memarg</code><br/>&nbsp;<br/>&nbsp;<br/>&nbsp; | <code>MEM0[𝑚 + <var>p1</var><sub>i32</sub>]<sub>i8</sub> ← <var>p2</var><sub>i8x16</sub>[𝑛]</code> <br/>&nbsp;<br/>&nbsp;<br/>&nbsp; |
| <code>2</code><br/>&nbsp;<br/>&nbsp;<br/>&nbsp; | | <code>v128\.store16\_lane</code><br/><code>i16x8\.store\_lane</code><br/><code>s16x8\.store\_lane</code><br/><code>u16x8\.store\_lane</code> | <code>lane,memarg</code><br/>&nbsp;<br/>&nbsp;<br/>&nbsp; | <code>MEM0[𝑚 + <var>p1</var><sub>i32</sub>]<sub>i16</sub> ← <var>p2</var><sub>i16x8</sub>[𝑛]</code> <br/>&nbsp;<br/>&nbsp;<br/>&nbsp; |
| <code>2</code><br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp; | | <code>v128\.store32\_lane</code><br/><code>i32x4\.store\_lane</code><br/><code>s32x4\.store\_lane</code><br/><code>u32x4\.store\_lane</code><br/><code>f32x4\.store\_lane</code> | <code>lane,memarg</code><br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp; | <code>MEM0[𝑚 + <var>p1</var><sub>i32</sub>]<sub>i32</sub> ← <var>p2</var><sub>i32x4</sub>[𝑛]</code> <br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp; |
| <code>2</code><br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp; | | <code>v128\.store64\_lane</code><br/><code>i64x2\.store\_lane</code><br/><code>s64x2\.store\_lane</code><br/><code>u64x2\.store\_lane</code><br/><code>f64x2\.store\_lane</code> | <code>lane,memarg</code><br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp; | <code>MEM0[𝑚 + <var>p1</var><sub>i32</sub>]<sub>i64</sub> ← <var>p2</var><sub>i64x2</sub>[𝑛]</code> <br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp; |


## 数値型変換


### <code>i32</code>・<code>i64</code>間変換

| 引数 | 返値 | 命令 | OP | 備考 |
|:-:|:-:|:--|:--|:--|
| <code>1</code><br/>&nbsp;<br/>&nbsp;<br/>&nbsp; | <code>i64</code><br/>&nbsp;<br/>&nbsp;<br/>&nbsp; | <code>i64\.extend\_i32\_s</code><br/><code>i64\.extend\_s32</code><br/><code>s64\.extend\_i32</code><br/><code>s64\.extend\_s32</code> | | <code><var>r</var><sub>i64</sub> ← ext64s(<var>p1</var><sub>i32</sub>)</code> <br/>&nbsp;<br/>&nbsp;<br/>&nbsp; |
| <code>1</code><br/>&nbsp;<br/>&nbsp;<br/>&nbsp; | <code>i64</code><br/>&nbsp;<br/>&nbsp;<br/>&nbsp; | <code>i64\.extend\_i32\_u</code><br/><code>i64\.extend\_u32</code><br/><code>u64\.extend\_i32</code><br/><code>u64\.extend\_u32</code> | | <code><var>r</var><sub>i64</sub> ← ext64u(<var>p1</var><sub>i32</sub>)</code> <br/>&nbsp;<br/>&nbsp;<br/>&nbsp; |
| | | | | |
| <code>1</code><br/>&nbsp;<br/>&nbsp; | <code>i32</code><br/>&nbsp;<br/>&nbsp; | <code>i32\.wrap</code><br/><code>s32\.wrap</code><br/><code>u32\.wrap</code> | | <code><var>r</var><sub>i32</sub> ← <var>p1</var><sub>i64</sub> &amp; (2<sup>32</sup>-1)</code> <br/>&nbsp;<br/>&nbsp; |


### 符号拡張

| 引数 | 返値 | 命令 | OP | 備考 |
|:-:|:-:|:--|:--|:--|
| <code>1</code><br/>&nbsp; | <code>i32</code><br/>&nbsp; | <code>i32\.extend8\_s</code><br/><code>s32\.extend8</code> | | <code><var>r</var><sub>i32</sub> ← ext8s(<var>p1</var><sub>i32</sub>)</code> <br/>&nbsp; |
| <code>1</code><br/>&nbsp; | <code>i64</code><br/>&nbsp; | <code>i64\.extend8\_s</code><br/><code>s64\.extend8</code> | | <code><var>r</var><sub>i64</sub> ← ext8s(<var>p1</var><sub>i64</sub>)</code> <br/>&nbsp; |
| <code>1</code><br/>&nbsp; | <code>i32</code><br/>&nbsp; | <code>i32\.extend16\_s</code><br/><code>s32\.extend16</code> | | <code><var>r</var><sub>i32</sub> ← ext16s(<var>p1</var><sub>i32</sub>)</code> <br/>&nbsp; |
| <code>1</code><br/>&nbsp; | <code>i64</code><br/>&nbsp; | <code>i64\.extend16\_s</code><br/><code>s64\.extend16</code> | | <code><var>r</var><sub>i64</sub> ← ext16s(<var>p1</var><sub>i64</sub>)</code> <br/>&nbsp; |
| <code>1</code><br/>&nbsp; | <code>i64</code><br/>&nbsp; | <code>i64\.extend32\_s</code><br/><code>s64\.extend32</code> | | <code><var>r</var><sub>i64</sub> ← ext32s(<var>p1</var><sub>i64</sub>)</code> <br/>&nbsp; |
| | | | | |
| <code>1</code><br/>&nbsp;<br/>&nbsp;<br/>&nbsp; | <code>v128</code><br/>&nbsp;<br/>&nbsp;<br/>&nbsp; | <code>i16x8\.extend\_low\_i8x16\_s</code><br/><code>i16x8\.extend\_low\_s8x16</code><br/><code>s16x8\.extend\_low\_i8x16</code><br/><code>s16x8\.extend\_low\_s8x16</code> | | <code><var>r</var><sub>i16x8</sub>[𝑛:0..7] ← ext8s(<var>p1</var><sub>i8x16</sub>[2𝑛+0])</code> <br/>&nbsp;<br/>&nbsp;<br/>&nbsp; |
| <code>1</code><br/>&nbsp;<br/>&nbsp;<br/>&nbsp; | <code>v128</code><br/>&nbsp;<br/>&nbsp;<br/>&nbsp; | <code>i16x8\.extend\_high\_i8x16\_s</code><br/><code>i16x8\.extend\_high\_s8x16</code><br/><code>s16x8\.extend\_high\_i8x16</code><br/><code>s16x8\.extend\_high\_s8x16</code> | | <code><var>r</var><sub>i16x8</sub>[𝑛:0..7] ← ext8s(<var>p1</var><sub>i8x16</sub>[2𝑛+1])</code> <br/>&nbsp;<br/>&nbsp;<br/>&nbsp; |
| <code>1</code><br/>&nbsp;<br/>&nbsp;<br/>&nbsp; | <code>v128</code><br/>&nbsp;<br/>&nbsp;<br/>&nbsp; | <code>i32x4\.extend\_low\_i16x8\_s</code><br/><code>i32x4\.extend\_low\_s16x8</code><br/><code>s32x4\.extend\_low\_i16x8</code><br/><code>s32x4\.extend\_low\_s16x8</code> | | <code><var>r</var><sub>i32x4</sub>[𝑛:0..3] ← ext16s(<var>p1</var><sub>i16x8</sub>[2𝑛+0])</code> <br/>&nbsp;<br/>&nbsp;<br/>&nbsp; |
| <code>1</code><br/>&nbsp;<br/>&nbsp;<br/>&nbsp; | <code>v128</code><br/>&nbsp;<br/>&nbsp;<br/>&nbsp; | <code>i32x4\.extend\_high\_i16x8\_s</code><br/><code>i32x4\.extend\_high\_s16x8</code><br/><code>s32x4\.extend\_high\_i16x8</code><br/><code>s32x4\.extend\_high\_s16x8</code> | | <code><var>r</var><sub>i32x4</sub>[𝑛:0..3] ← ext16s(<var>p1</var><sub>i16x8</sub>[2𝑛+1])</code> <br/>&nbsp;<br/>&nbsp;<br/>&nbsp; |
| <code>1</code><br/>&nbsp;<br/>&nbsp;<br/>&nbsp; | <code>v128</code><br/>&nbsp;<br/>&nbsp;<br/>&nbsp; | <code>i64x2\.extend\_low\_i32x4\_s</code><br/><code>i64x2\.extend\_low\_s32x4</code><br/><code>s64x2\.extend\_low\_i32x4</code><br/><code>s64x2\.extend\_low\_s32x4</code> | | <code><var>r</var><sub>i64x2</sub>[𝑛:0..1] ← ext32s(<var>p1</var><sub>i32x4</sub>[2𝑛+0])</code> <br/>&nbsp;<br/>&nbsp;<br/>&nbsp; |
| <code>1</code><br/>&nbsp;<br/>&nbsp;<br/>&nbsp; | <code>v128</code><br/>&nbsp;<br/>&nbsp;<br/>&nbsp; | <code>i64x2\.extend\_high\_i32x4\_s</code><br/><code>i64x2\.extend\_high\_s32x4</code><br/><code>s64x2\.extend\_high\_i32x4</code><br/><code>s64x2\.extend\_high\_s32x4</code> | | <code><var>r</var><sub>i64x2</sub>[𝑛:0..1] ← ext32s(<var>p1</var><sub>i32x4</sub>[2𝑛+1])</code> <br/>&nbsp;<br/>&nbsp;<br/>&nbsp; |


### ゼロ拡張

| 引数 | 返値 | 命令 | OP | 備考 |
|:-:|:-:|:--|:--|:--|
| <code>1</code><br/>&nbsp;<br/>&nbsp;<br/>&nbsp; | <code>v128</code><br/>&nbsp;<br/>&nbsp;<br/>&nbsp; | <code>i16x8\.extend\_low\_i8x16\_u</code><br/><code>i16x8\.extend\_low\_u8x16</code><br/><code>u16x8\.extend\_low\_i8x16</code><br/><code>u16x8\.extend\_low\_u8x16</code> | | <code><var>r</var><sub>i16x8</sub>[𝑛:0..7] ← ext8u(<var>p1</var><sub>i8x16</sub>[2𝑛+0])</code> <br/>&nbsp;<br/>&nbsp;<br/>&nbsp; |
| <code>1</code><br/>&nbsp;<br/>&nbsp;<br/>&nbsp; | <code>v128</code><br/>&nbsp;<br/>&nbsp;<br/>&nbsp; | <code>i16x8\.extend\_high\_i8x16\_u</code><br/><code>i16x8\.extend\_high\_u8x16</code><br/><code>u16x8\.extend\_high\_i8x16</code><br/><code>u16x8\.extend\_high\_u8x16</code> | | <code><var>r</var><sub>i16x8</sub>[𝑛:0..7] ← ext8u(<var>p1</var><sub>i8x16</sub>[2𝑛+1])</code> <br/>&nbsp;<br/>&nbsp;<br/>&nbsp; |
| <code>1</code><br/>&nbsp;<br/>&nbsp;<br/>&nbsp; | <code>v128</code><br/>&nbsp;<br/>&nbsp;<br/>&nbsp; | <code>i32x4\.extend\_low\_i16x8\_u</code><br/><code>i32x4\.extend\_low\_u16x8</code><br/><code>u32x4\.extend\_low\_i16x8</code><br/><code>u32x4\.extend\_low\_u16x8</code> | | <code><var>r</var><sub>i32x4</sub>[𝑛:0..3] ← ext16u(<var>p1</var><sub>i16x8</sub>[2𝑛+0])</code> <br/>&nbsp;<br/>&nbsp;<br/>&nbsp; |
| <code>1</code><br/>&nbsp;<br/>&nbsp;<br/>&nbsp; | <code>v128</code><br/>&nbsp;<br/>&nbsp;<br/>&nbsp; | <code>i32x4\.extend\_high\_i16x8\_u</code><br/><code>i32x4\.extend\_high\_u16x8</code><br/><code>u32x4\.extend\_high\_i16x8</code><br/><code>u32x4\.extend\_high\_u16x8</code> | | <code><var>r</var><sub>i32x4</sub>[𝑛:0..3] ← ext16u(<var>p1</var><sub>i16x8</sub>[2𝑛+1])</code> <br/>&nbsp;<br/>&nbsp;<br/>&nbsp; |
| <code>1</code><br/>&nbsp;<br/>&nbsp;<br/>&nbsp; | <code>v128</code><br/>&nbsp;<br/>&nbsp;<br/>&nbsp; | <code>i64x2\.extend\_low\_i32x4\_u</code><br/><code>i64x2\.extend\_low\_u32x4</code><br/><code>u64x2\.extend\_low\_i32x4</code><br/><code>u64x2\.extend\_low\_u32x4</code> | | <code><var>r</var><sub>i64x2</sub>[𝑛:0..1] ← ext32u(<var>p1</var><sub>i32x4</sub>[2𝑛+0])</code> <br/>&nbsp;<br/>&nbsp;<br/>&nbsp; |
| <code>1</code><br/>&nbsp;<br/>&nbsp;<br/>&nbsp; | <code>v128</code><br/>&nbsp;<br/>&nbsp;<br/>&nbsp; | <code>i64x2\.extend\_high\_i32x4\_u</code><br/><code>i64x2\.extend\_high\_u32x4</code><br/><code>u64x2\.extend\_high\_i32x4</code><br/><code>u64x2\.extend\_high\_u32x4</code> | | <code><var>r</var><sub>i64x2</sub>[𝑛:0..1] ← ext32u(<var>p1</var><sub>i32x4</sub>[2𝑛+1])</code> <br/>&nbsp;<br/>&nbsp;<br/>&nbsp; |


### ビット幅縮小

| 引数 | 返値 | 命令 | OP | 備考 |
|:-:|:-:|:--|:--|:--|
| <code>2</code><br/>&nbsp;<br/>&nbsp;<br/>&nbsp; | <code>v128</code><br/>&nbsp;<br/>&nbsp;<br/>&nbsp; | <code>i8x16\.narrow\_i16x8\_s</code><br/><code>i8x16\.narrow\_s16x8</code><br/><code>s8x16\.narrow\_i16x8</code><br/><code>s8x16\.narrow\_s16x8</code> | | <code><var>r</var><sub>i8x16</sub>[𝑛:0..15] ← sat8s(<var>t</var><sub>s16x8</sub>[𝑛 &gt;&gt; 1])</code><br/><code>    <var>t</var><sub>s16x8</sub> = (𝑛 &amp; 1) ? <var>p2</var><sub>s16x8</sub> : <var>p1</var><sub>s16x8</sub></code> <br/>&nbsp;<br/>&nbsp; |
| <code>2</code><br/>&nbsp;<br/>&nbsp;<br/>&nbsp; | <code>v128</code><br/>&nbsp;<br/>&nbsp;<br/>&nbsp; | <code>i8x16\.narrow\_i16x8\_u</code><br/><code>i8x16\.narrow\_u16x8</code><br/><code>u8x16\.narrow\_i16x8</code><br/><code>u8x16\.narrow\_u16x8</code> | | <code><var>r</var><sub>i8x16</sub>[𝑛:0..15] ← sat8u(<var>t</var><sub>u16x8</sub>[𝑛 &gt;&gt; 1])</code><br/><code>    <var>t</var><sub>u16x8</sub> = (𝑛 &amp; 1) ? <var>p2</var><sub>u16x8</sub> : <var>p1</var><sub>u16x8</sub></code><br/>&nbsp;<br/>&nbsp; |
| <code>2</code><br/>&nbsp;<br/>&nbsp;<br/>&nbsp; | <code>v128</code><br/>&nbsp;<br/>&nbsp;<br/>&nbsp; | <code>i16x8\.narrow\_i32x4\_s</code><br/><code>i16x8\.narrow\_s32x4</code><br/><code>s16x8\.narrow\_i32x4</code><br/><code>s16x8\.narrow\_s32x4</code> | | <code><var>r</var><sub>i16x8</sub>[𝑛:0..7] ← sat16s(<var>t</var><sub>s32x4</sub>[𝑛 &gt;&gt; 1])</code><br/><code>    <var>t</var><sub>s32x4</sub> = (𝑛 &amp; 1) ? <var>p2</var><sub>s32x4</sub> : <var>p1</var><sub>s32x4</sub></code> <br/>&nbsp;<br/>&nbsp; |
| <code>2</code><br/>&nbsp;<br/>&nbsp;<br/>&nbsp; | <code>v128</code><br/>&nbsp;<br/>&nbsp;<br/>&nbsp; | <code>i16x8\.narrow\_i32x4\_u</code><br/><code>i16x8\.narrow\_u32x4</code><br/><code>u16x8\.narrow\_i32x4</code><br/><code>u16x8\.narrow\_u32x4</code> | | <code><var>r</var><sub>i16x8</sub>[𝑛:0..7] ← sat16u(<var>t</var><sub>u32x4</sub>[𝑛 &gt;&gt; 1])</code><br/><code>    <var>t</var><sub>u32x4</sub> = (𝑛 &amp; 1) ? <var>p2</var><sub>u32x4</sub> : <var>p1</var><sub>u32x4</sub></code> <br/>&nbsp;<br/>&nbsp; |


### 整数型と浮動小数点型の変換


#### 整数化(飽和処理なし)

| 引数 | 返値 | 命令 | OP | 備考 |
|:-:|:-:|:--|:--|:--|
| <code>1</code><br/>&nbsp; | <code>i32</code><br/>&nbsp; | <code>i32\.trunc\_f32\_s</code><br/><code>s32\.trunc\_f32</code> | | <code><var>r</var><sub>s32</sub> ← trunc(<var>p1</var><sub>f32</sub>)</code> <br/>&nbsp; |
| <code>1</code><br/>&nbsp; | <code>i32</code><br/>&nbsp; | <code>i32\.trunc\_f32\_u</code><br/><code>u32\.trunc\_f32</code> | | <code><var>r</var><sub>u32</sub> ← trunc(<var>p1</var><sub>f32</sub>)</code> <br/>&nbsp; |
| <code>1</code><br/>&nbsp; | <code>i32</code><br/>&nbsp; | <code>i32\.trunc\_f64\_s</code><br/><code>s32\.trunc\_f64</code> | | <code><var>r</var><sub>s32</sub> ← trunc(<var>p1</var><sub>f64</sub>)</code> <br/>&nbsp; |
| <code>1</code><br/>&nbsp; | <code>i32</code><br/>&nbsp; | <code>i32\.trunc\_f64\_u</code><br/><code>u32\.trunc\_f64</code> | | <code><var>r</var><sub>u32</sub> ← trunc(<var>p1</var><sub>f64</sub>)</code> <br/>&nbsp; |
| <code>1</code><br/>&nbsp; | <code>i64</code><br/>&nbsp; | <code>i64\.trunc\_f32\_s</code><br/><code>s64\.trunc\_f32</code> | | <code><var>r</var><sub>s64</sub> ← trunc(<var>p1</var><sub>f32</sub>)</code> <br/>&nbsp; |
| <code>1</code><br/>&nbsp; | <code>i64</code><br/>&nbsp; | <code>i64\.trunc\_f32\_u</code><br/><code>u64\.trunc\_f32</code> | | <code><var>r</var><sub>u64</sub> ← trunc(<var>p1</var><sub>f32</sub>)</code> <br/>&nbsp; |
| <code>1</code><br/>&nbsp; | <code>i64</code><br/>&nbsp; | <code>i64\.trunc\_f64\_s</code><br/><code>s64\.trunc\_f64</code> | | <code><var>r</var><sub>s64</sub> ← trunc(<var>p1</var><sub>f64</sub>)</code> <br/>&nbsp; |
| <code>1</code><br/>&nbsp; | <code>i64</code><br/>&nbsp; | <code>i64\.trunc\_f64\_u</code><br/><code>u64\.trunc\_f64</code> | | <code><var>r</var><sub>u64</sub> ← trunc(<var>p1</var><sub>f64</sub>)</code> <br/>&nbsp; |


#### 整数化(飽和処理あり)

| 引数 | 返値 | 命令 | OP | 備考 |
|:-:|:-:|:--|:--|:--|
| <code>1</code><br/>&nbsp; | <code>i32</code><br/>&nbsp; | <code>i32\.trunc\_sat\_f32\_s</code><br/><code>s32\.trunc\_sat\_f32</code> | | <code><var>r</var><sub>i32</sub> ← trunc(sat32s(<var>p1</var><sub>f32</sub>))</code> <br/>&nbsp; |
| <code>1</code><br/>&nbsp; | <code>i32</code><br/>&nbsp; | <code>i32\.trunc\_sat\_f32\_u</code><br/><code>u32\.trunc\_sat\_f32</code> | | <code><var>r</var><sub>i32</sub> ← trunc(sat32u(<var>p1</var><sub>f32</sub>))</code> <br/>&nbsp; |
| <code>1</code><br/>&nbsp; | <code>i32</code><br/>&nbsp; | <code>i32\.trunc\_sat\_f64\_s</code><br/><code>s32\.trunc\_sat\_f64</code> | | <code><var>r</var><sub>i32</sub> ← trunc(sat32s(<var>p1</var><sub>f64</sub>))</code> <br/>&nbsp; |
| <code>1</code><br/>&nbsp; | <code>i32</code><br/>&nbsp; | <code>i32\.trunc\_sat\_f64\_u</code><br/><code>u32\.trunc\_sat\_f64</code> | | <code><var>r</var><sub>i32</sub> ← trunc(sat32u(<var>p1</var><sub>f64</sub>))</code> <br/>&nbsp; |
| <code>1</code><br/>&nbsp; | <code>i64</code><br/>&nbsp; | <code>i64\.trunc\_sat\_f32\_s</code><br/><code>s64\.trunc\_sat\_f32</code> | | <code><var>r</var><sub>i64</sub> ← trunc(sat64s(<var>p1</var><sub>f32</sub>))</code> <br/>&nbsp; |
| <code>1</code><br/>&nbsp; | <code>i64</code><br/>&nbsp; | <code>i64\.trunc\_sat\_f32\_u</code><br/><code>u64\.trunc\_sat\_f32</code> | | <code><var>r</var><sub>i64</sub> ← trunc(sat64u(<var>p1</var><sub>f32</sub>))</code> <br/>&nbsp; |
| <code>1</code><br/>&nbsp; | <code>i64</code><br/>&nbsp; | <code>i64\.trunc\_sat\_f64\_s</code><br/><code>s64\.trunc\_sat\_f64</code> | | <code><var>r</var><sub>i64</sub> ← trunc(sat64s(<var>p1</var><sub>f64</sub>))</code> <br/>&nbsp; |
| <code>1</code><br/>&nbsp; | <code>i64</code><br/>&nbsp; | <code>i64\.trunc\_sat\_f64\_u</code><br/><code>u64\.trunc\_sat\_f64</code> | | <code><var>r</var><sub>i64</sub> ← trunc(sat64u(<var>p1</var><sub>f64</sub>))</code> <br/>&nbsp; |
| | | | | |
| <code>1</code><br/>&nbsp; | <code>v128</code><br/>&nbsp; | <code>i32x4\.trunc\_sat\_f32x4\_s</code><br/><code>s32x4\.trunc\_sat\_f32x4</code> | | <code><var>r</var><sub>i32x4</sub>[𝑛:0..3] ← trunc(sat32s(<var>p1</var><sub>f32x4</sub>[𝑛]))</code> <br/>&nbsp; |
| <code>1</code><br/>&nbsp; | <code>v128</code><br/>&nbsp; | <code>i32x4\.trunc\_sat\_f32x4\_u</code><br/><code>u32x4\.trunc\_sat\_f32x4</code> | | <code><var>r</var><sub>i32x4</sub>[𝑛:0..3] ← trunc(sat32u(<var>p1</var><sub>f32x4</sub>[𝑛]))</code> <br/>&nbsp; |
| <code>1</code><br/>&nbsp; | <code>v128</code><br/>&nbsp; | <code>i32x4\.trunc\_sat\_f64x2\_s\_zero</code><br/><code>s32x4\.trunc\_sat\_f64x2\_zero</code> | | <code><var>r</var><sub>i32x4</sub>[𝑛:0..3] ← trunc(sat64s(<var>t</var><sub>f64</sub>))</code><br/><code>    <var>t</var><sub>f64</sub> = 𝑛 &lt; 2 ? <var>p1</var><sub>f64x2</sub>[𝑛] : 0</code> |
| <code>1</code><br/>&nbsp; | <code>v128</code><br/>&nbsp; | <code>i32x4\.trunc\_sat\_f64x2\_u\_zero</code><br/><code>u32x4\.trunc\_sat\_f64x2\_zero</code> | | <code><var>r</var><sub>i32x4</sub>[𝑛:0..3] ← trunc(sat64u(<var>t</var><sub>f64</sub>))</code><br/><code>    <var>t</var><sub>f64</sub> = 𝑛 &lt; 2 ? <var>p1</var><sub>f64x2</sub>[𝑛] : 0</code> |


#### 浮動小数点化

| 引数 | 返値 | 命令 | OP | 備考 |
|:-:|:-:|:--|:--|:--|
| <code>1</code><br/>&nbsp; | <code>f32</code><br/>&nbsp; | <code>f32\.convert\_i32\_s</code><br/><code>f32\.convert\_s32</code> | | <code><var>r</var><sub>f32</sub> ← f32(<var>p1</var><sub>s32</sub>)</code> <br/>&nbsp; |
| <code>1</code><br/>&nbsp; | <code>f32</code><br/>&nbsp; | <code>f32\.convert\_i32\_u</code><br/><code>f32\.convert\_u32</code> | | <code><var>r</var><sub>f32</sub> ← f32(<var>p1</var><sub>u32</sub>)</code> <br/>&nbsp; |
| <code>1</code><br/>&nbsp; | <code>f32</code><br/>&nbsp; | <code>f32\.convert\_i64\_s</code><br/><code>f32\.convert\_s64</code> | | <code><var>r</var><sub>f32</sub> ← f32(<var>p1</var><sub>s64</sub>)</code> <br/>&nbsp; |
| <code>1</code><br/>&nbsp; | <code>f32</code><br/>&nbsp; | <code>f32\.convert\_i64\_u</code><br/><code>f32\.convert\_u64</code> | | <code><var>r</var><sub>f32</sub> ← f32(<var>p1</var><sub>u64</sub>)</code> <br/>&nbsp; |
| <code>1</code><br/>&nbsp; | <code>f64</code><br/>&nbsp; | <code>f64\.convert\_i32\_s</code><br/><code>f64\.convert\_s32</code> | | <code><var>r</var><sub>f64</sub> ← f64(<var>p1</var><sub>s32</sub>)</code> <br/>&nbsp; |
| <code>1</code><br/>&nbsp; | <code>f64</code><br/>&nbsp; | <code>f64\.convert\_i32\_u</code><br/><code>f64\.convert\_u32</code> | | <code><var>r</var><sub>f64</sub> ← f64(<var>p1</var><sub>u32</sub>)</code> <br/>&nbsp; |
| <code>1</code><br/>&nbsp; | <code>f64</code><br/>&nbsp; | <code>f64\.convert\_i64\_s</code><br/><code>f64\.convert\_s64</code> | | <code><var>r</var><sub>f64</sub> ← f64(<var>p1</var><sub>s64</sub>)</code> <br/>&nbsp; |
| <code>1</code><br/>&nbsp; | <code>f64</code><br/>&nbsp; | <code>f64\.convert\_i64\_u</code><br/><code>f64\.convert\_u64</code> | | <code><var>r</var><sub>f64</sub> ← f64(<var>p1</var><sub>u64</sub>)</code> <br/>&nbsp; |
| | | | | |
| <code>1</code><br/>&nbsp; | <code>v128</code><br/>&nbsp; | <code>f32x4\.convert\_i32x4\_s</code><br/><code>f32x4\.convert\_s32x4</code> | | <code><var>r</var><sub>f32x4</sub>[𝑛:0..3] ← f32(<var>p1</var><sub>s32x4</sub>[𝑛])</code> <br/>&nbsp; |
| <code>1</code><br/>&nbsp; | <code>v128</code><br/>&nbsp; | <code>f32x4\.convert\_i32x4\_u</code><br/><code>f32x4\.convert\_u32x4</code> | | <code><var>r</var><sub>f32x4</sub>[𝑛:0..3] ← f32(<var>p1</var><sub>u32x4</sub>[𝑛])</code> <br/>&nbsp; |
| <code>1</code><br/>&nbsp; | <code>v128</code><br/>&nbsp; | <code>f64x2\.convert\_low\_i32x4\_s</code><br/><code>f64x2\.convert\_low\_s32x4</code> | | <code><var>r</var><sub>f64x2</sub>[𝑛:0..1] ← f64(<var>p1</var><sub>s32x4</sub>[2𝑛])</code> <br/>&nbsp; |
| <code>1</code><br/>&nbsp; | <code>v128</code><br/>&nbsp; | <code>f64x2\.convert\_low\_i32x4\_u</code><br/><code>f64x2\.convert\_low\_u32x4</code> | | <code><var>r</var><sub>f64x2</sub>[𝑛:0..1] ← f64(<var>p1</var><sub>u32x4</sub>[2𝑛])</code> <br/>&nbsp; |


#### ビット複写型変換

| 引数 | 返値 | 命令 | OP | 備考 |
|:-:|:-:|:--|:--|:--|
| <code>1</code><br/>&nbsp;<br/>&nbsp; | <code>i32</code><br/>&nbsp;<br/>&nbsp; | <code>i32\.reinterpret\_f32</code><br/><code>s32\.reinterpret\_f32</code><br/><code>u32\.reinterpret\_f32</code> | | <code><var>r</var><sub>i32</sub> ← bit\_cast(<var>p1</var><sub>f32</sub></code>) <br/>&nbsp;<br/>&nbsp; |
| <code>1</code><br/>&nbsp;<br/>&nbsp; | <code>i64</code><br/>&nbsp;<br/>&nbsp; | <code>i64\.reinterpret\_f64</code><br/><code>s64\.reinterpret\_f64</code><br/><code>u64\.reinterpret\_f64</code> | | <code><var>r</var><sub>i64</sub> ← bit\_cast(<var>p1</var><sub>f64</sub></code>) <br/>&nbsp;<br/>&nbsp; |
| <code>1</code><br/>&nbsp;<br/>&nbsp; | <code>f32</code><br/>&nbsp;<br/>&nbsp; | <code>f32\.reinterpret\_i32</code><br/><code>f32\.reinterpret\_s32</code><br/><code>f32\.reinterpret\_u32</code> | | <code><var>r</var><sub>f32</sub> ← bit\_cast(<var>p1</var><sub>i32</sub></code>) <br/>&nbsp;<br/>&nbsp; |
| <code>1</code><br/>&nbsp;<br/>&nbsp; | <code>f64</code><br/>&nbsp;<br/>&nbsp; | <code>f64\.reinterpret\_i64</code><br/><code>f64\.reinterpret\_s64</code><br/><code>f64\.reinterpret\_u64</code> | | <code><var>r</var><sub>f64</sub> ← bit\_cast(<var>p1</var><sub>i64</sub></code>) <br/>&nbsp;<br/>&nbsp; |

<code>bit_cast(x)</code>の出力はバイナリ レベルでは <code>x</code> と同一です。

例: <code>bit\_cast(0x3f800000<sub>i32</sub>) → 1.0<sub>f32</sub></code>


#### 単精度と倍精度の変換

| 引数 | 返値 | 命令 | OP | 備考 |
|:-:|:-:|:--|:--|:--|
| <code>1</code> | <code>f32</code> | <code>f32\.demote\_f64</code> | | <code><var>r</var><sub>f32</sub> ← f32(<var>p1</var><sub>f64</sub>)</code> |
| <code>1</code> | <code>f64</code> | <code>f64\.promote\_f32</code> | | <code><var>r</var><sub>f64</sub> ← f64(<var>p1</var><sub>f32</sub>)</code> |
| | | | | |
| <code>1</code> | <code>v128</code> | <code>f32x4\.demote\_f64x2\_zero</code> | | <code><var>r</var><sub>f32x4</sub>[𝑛:0..3] ← f32(<var>p1</var><sub>f64x2</sub>[𝑛] ?? 0)</code> |
| <code>1</code> | <code>v128</code> | <code>f64x2\.promote\_low\_f32x4</code> | | <code><var>r</var><sub>f64x2</sub>[𝑛:0..1] ← f64(<var>p1</var><sub>f32x4</sub>[2𝑛])</code> |


### 数値型とベクトル型


#### ベクトルの全要素に同一値を設定

| 引数 | 返値 | 命令 | OP | 備考 |
|:-:|:-:|:--|:--|:--|
| <code>1</code><br/>&nbsp;<br/>&nbsp; | <code>v128</code><br/>&nbsp;<br/>&nbsp; | <code>i8x16\.splat</code><br/><code>s8x16\.splat</code><br/><code>u8x16\.splat</code> | | <code><var>r</var><sub>i8x16</sub>[0..15] ← (<var>p1</var><sub>i32</sub> &amp; 0xff)</code> <br/>&nbsp;<br/>&nbsp; |
| <code>1</code><br/>&nbsp;<br/>&nbsp; | <code>v128</code><br/>&nbsp;<br/>&nbsp; | <code>i16x8\.splat</code><br/><code>s16x8\.splat</code><br/><code>u16x8\.splat</code> | | <code><var>r</var><sub>i16x8</sub>[0..7] ← (<var>p1</var><sub>i32</sub> &amp; 0xffff)</code> <br/>&nbsp;<br/>&nbsp; |
| <code>1</code><br/>&nbsp;<br/>&nbsp; | <code>v128</code><br/>&nbsp;<br/>&nbsp; | <code>i32x4\.splat</code><br/><code>s32x4\.splat</code><br/><code>u32x4\.splat</code> | | <code><var>r</var><sub>i32x4</sub>[0..3] ← <var>p1</var><sub>i32</sub></code> <br/>&nbsp;<br/>&nbsp; |
| <code>1</code><br/>&nbsp;<br/>&nbsp; | <code>v128</code><br/>&nbsp;<br/>&nbsp; | <code>i64x2\.splat</code><br/><code>s64x2\.splat</code><br/><code>u64x2\.splat</code> | | <code><var>r</var><sub>i64x2</sub>[0..1] ← <var>p1</var><sub>i64</sub></code> <br/>&nbsp;<br/>&nbsp; |
| <code>1</code> | <code>v128</code> | <code>f32x4\.splat</code> | | <code><var>r</var><sub>f32x4</sub>[0..3] ← <var>p1</var><sub>f32</sub></code> |
| <code>1</code> | <code>v128</code> | <code>f64x2\.splat</code> | | <code><var>r</var><sub>f64x2</sub>[0..1] ← <var>p1</var><sub>f64</sub></code> |


#### ベクトル要素の取得と設定

| 引数 | 返値 | 命令 | OP | 備考 (<code>𝑛 = lane</code>) |
|:-:|:-:|:--|:--|:--|
| <code>1</code><br/>&nbsp; | <code>i32</code><br/>&nbsp; | <code>i8x16\.extract\_lane\_s</code><br/><code>s8x16\.extract\_lane</code> | <code>lane</code><br/>&nbsp; | <code><var>r<sub>i32</sub></var> ← ext8s(<var>p1</var><sub>i8x16</sub>[𝑛])</code> <br/>&nbsp; |
| <code>1</code><br/>&nbsp; | <code>i32</code><br/>&nbsp; | <code>i8x16\.extract\_lane\_u</code><br/><code>u8x16\.extract\_lane</code> | <code>lane</code><br/>&nbsp; | <code><var>r<sub>i32</sub></var> ← ext8u(<var>p1</var><sub>i8x16</sub>[𝑛])</code> <br/>&nbsp; |
| <code>1</code><br/>&nbsp; | <code>i32</code><br/>&nbsp; | <code>i16x8\.extract\_lane\_s</code><br/><code>s16x8\.extract\_lane</code> | <code>lane</code><br/>&nbsp; | <code><var>r<sub>i32</sub></var> ← ext16s(<var>p1</var><sub>i16x8</sub>[𝑛])</code> <br/>&nbsp; |
| <code>1</code><br/>&nbsp; | <code>i32</code><br/>&nbsp; | <code>i16x8\.extract\_lane\_u</code><br/><code>u16x8\.extract\_lane</code> | <code>lane</code><br/>&nbsp; | <code><var>r<sub>i32</sub></var> ← ext16u(<var>p1</var><sub>i16x8</sub>[𝑛])</code> <br/>&nbsp; |
| <code>1</code><br/>&nbsp;<br/>&nbsp; | <code>i32</code><br/>&nbsp;<br/>&nbsp; | <code>i32x4\.extract\_lane</code><br/><code>s32x4\.extract\_lane</code><br/><code>u32x4\.extract\_lane</code> | <code>lane</code><br/>&nbsp;<br/>&nbsp; | <code><var>r<sub>i32</sub></var> ← <var>p1</var><sub>i32x4</sub>[𝑛]</code> <br/>&nbsp;<br/>&nbsp; |
| <code>1</code><br/>&nbsp;<br/>&nbsp; | <code>i64</code><br/>&nbsp;<br/>&nbsp; | <code>i64x2\.extract\_lane</code><br/><code>s64x2\.extract\_lane</code><br/><code>u64x2\.extract\_lane</code> | <code>lane</code><br/>&nbsp;<br/>&nbsp; | <code><var>r<sub>i64</sub></var> ← <var>p1</var><sub>i64x2</sub>[𝑛]</code> <br/>&nbsp;<br/>&nbsp; |
| <code>1</code> | <code>f32</code> | <code>f32x4\.extract\_lane</code> | <code>lane</code> | <code><var>r<sub>f32</sub></var> ← <var>p1</var><sub>f32x4</sub>[𝑛]</code> |
| <code>1</code> | <code>f64</code> | <code>f64x2\.extract\_lane</code> | <code>lane</code> | <code><var>r<sub>f64</sub></var> ← <var>p1</var><sub>f64x2</sub>[𝑛]</code> |
| | | | | |
| <code>2</code><br/>&nbsp;<br/>&nbsp; | <code>v128</code><br/>&nbsp;<br/>&nbsp; | <code>i8x16\.replace\_lane</code><br/><code>s8x16\.replace\_lane</code><br/><code>u8x16\.replace\_lane</code> | <code>lane</code><br/>&nbsp;<br/>&nbsp; | <code><var>r</var><sub>i8x16</sub> ← <var>p1</var><sub>v128</sub></code><br><code><var>r</var><sub>i8x16</sub>[lane] ← <var>p2</var><sub>i32</sub></code> <br/>&nbsp; |
| <code>2</code><br/>&nbsp;<br/>&nbsp; | <code>v128</code><br/>&nbsp;<br/>&nbsp; | <code>i16x8\.replace\_lane</code><br/><code>s16x8\.replace\_lane</code><br/><code>u16x8\.replace\_lane</code> | <code>lane</code><br/>&nbsp;<br/>&nbsp; | <code><var>r</var><sub>i16x8</sub> ← <var>p1</var><sub>v128</sub></code><br><code><var>r</var><sub>i16x8</sub>[lane] ← <var>p2</var><sub>i32</sub></code> <br/>&nbsp; |
| <code>2</code><br/>&nbsp;<br/>&nbsp; | <code>v128</code><br/>&nbsp;<br/>&nbsp; | <code>i32x4\.replace\_lane</code><br/><code>s32x4\.replace\_lane</code><br/><code>u32x4\.replace\_lane</code> | <code>lane</code><br/>&nbsp;<br/>&nbsp; | <code><var>r</var><sub>i32x4</sub> ← <var>p1</var><sub>v128</sub></code><br><code><var>r</var><sub>i32x4</sub>[lane] ← <var>p2</var><sub>i32</sub></code> <br/>&nbsp; |
| <code>2</code><br/>&nbsp;<br/>&nbsp; | <code>v128</code><br/>&nbsp;<br/>&nbsp; | <code>i64x2\.replace\_lane</code><br/><code>s64x2\.replace\_lane</code><br/><code>u64x2\.replace\_lane</code> | <code>lane</code><br/>&nbsp;<br/>&nbsp; | <code><var>r</var><sub>i64x2</sub> ← <var>p1</var><sub>v128</sub></code><br><code><var>r</var><sub>i64x2</sub>[lane] ← <var>p2</var><sub>i64</sub></code> <br/>&nbsp; |
| <code>2</code><br/>&nbsp; | <code>v128</code><br/>&nbsp; | <code>f32x4\.replace\_lane</code><br/>&nbsp; | <code>lane</code><br/>&nbsp; | <code><var>r</var><sub>f32x4</sub> ← <var>p1</var><sub>v128</sub></code><br><code><var>r</var><sub>f32x4</sub>[lane] ← <var>p2</var><sub>f32</sub></code> |
| <code>2</code><br/>&nbsp; | <code>v128</code><br/>&nbsp; | <code>f64x2\.replace\_lane</code><br/>&nbsp; | <code>lane</code><br/>&nbsp; | <code><var>r</var><sub>f64x2</sub> ← <var>p1</var><sub>v128</sub></code><br><code><var>r</var><sub>f64x2</sub>[lane] ← <var>p2</var><sub>f64</sub></code> |


### 混成

| 引数 | 返値 | 命令 | OP | 備考 |
|:-:|:-:|:--|:--|:--|
| <code>2</code><br/>&nbsp;<br/>&nbsp; | <code>v128</code><br/>&nbsp;<br/>&nbsp; | <code>i8x16\.shuffle</code><br/><code>s8x16\.shuffle</code><br/><code>u8x16\.shuffle</code> | <code>lane<sub>0</sub>,...,lane<sub>15</sub></code><br/>&nbsp;<br/>&nbsp; | <code><var>r</var><sub>i8x16</sub>[𝑛:0..15] ← data[lane<sub>𝑛</sub>]</code><br/><code>    data = [...<var>p1</var><sub>i8x16</sub>, ...<var>p2</var><sub>i8x16</sub>] </code> <br/>&nbsp; |
| <code>2</code><br/>&nbsp;<br/>&nbsp; | <code>v128</code><br/>&nbsp;<br/>&nbsp; | <code>i8x16\.swizzle</code><br/><code>s8x16\.swizzle</code><br/><code>u8x16\.swizzle</code> | | <code><var>r</var><sub>i8x16</sub>[𝑛:0..15] ← <var>p1</var><sub>i8x16</sub>[<var>p2</var><sub>i8x16</sub>[𝑛]] ?? 0</code> <br/>&nbsp;<br/>&nbsp; |


## 数値比較


### 条件：一致

| 引数 | 返値 | 命令 | OP | 備考 |
|:-:|:-:|:--|:--|:--|
| <code>1</code><br/>&nbsp;<br/>&nbsp; | <code>i32</code><br/>&nbsp;<br/>&nbsp; | <code>i32\.eqz</code><br/><code>s32\.eqz</code><br/><code>u32\.eqz</code> | | <code><var>r</var><sub>i32</sub> ← (<var>p1</var><sub>i32</sub> == 0)</code> <br/>&nbsp;<br/>&nbsp; |
| <code>1</code><br/>&nbsp;<br/>&nbsp; | <code>i32</code><br/>&nbsp;<br/>&nbsp; | <code>i64\.eqz</code><br/><code>s64\.eqz</code><br/><code>u64\.eqz</code> | | <code><var>r</var><sub>i32</sub> ← (<var>p1</var><sub>i64</sub> == 0)</code> <br/>&nbsp;<br/>&nbsp; |
| <code>2</code><br/>&nbsp;<br/>&nbsp; | <code>i32</code><br/>&nbsp;<br/>&nbsp; | <code>i32\.eq</code><br/><code>s32\.eq</code><br/><code>u32\.eq</code> | | <code><var>r</var><sub>i32</sub> ← (<var>p1</var><sub>i32</sub> == <var>p2</var><sub>i32</sub>)</code> <br/>&nbsp;<br/>&nbsp; |
| <code>2</code><br/>&nbsp;<br/>&nbsp; | <code>i32</code><br/>&nbsp;<br/>&nbsp; | <code>i64\.eq</code><br/><code>s64\.eq</code><br/><code>u64\.eq</code> | | <code><var>r</var><sub>i32</sub> ← (<var>p1</var><sub>i64</sub> == <var>p2</var><sub>i64</sub>)</code> <br/>&nbsp;<br/>&nbsp; |
| <code>2</code> | <code>i32</code> | <code>f32\.eq</code> | | <code><var>r</var><sub>i32</sub> ← (<var>p1</var><sub>f32</sub> == <var>p2</var><sub>f32</sub>)</code> |
| <code>2</code> | <code>i32</code> | <code>f64\.eq</code> | | <code><var>r</var><sub>i32</sub> ← (<var>p1</var><sub>f64</sub> == <var>p2</var><sub>f64</sub>)</code> |
| | | | | |
| <code>2</code><br/>&nbsp;<br/>&nbsp; | <code>v128</code><br/>&nbsp;<br/>&nbsp; | <code>i8x16\.eq</code><br/><code>s8x16\.eq</code><br/><code>u8x16\.eq</code> | | <code><var>r</var><sub>i8x16</sub>[𝑛:0..15] ← (<var>p1</var><sub>i8x16</sub>[𝑛] == <var>p2</var><sub>i8x16</sub>[𝑛])</code> <br/>&nbsp;<br/>&nbsp; |
| <code>2</code><br/>&nbsp;<br/>&nbsp; | <code>v128</code><br/>&nbsp;<br/>&nbsp; | <code>i16x8\.eq</code><br/><code>s16x8\.eq</code><br/><code>u16x8\.eq</code> | | <code><var>r</var><sub>i16x8</sub>[𝑛:0..7] ← (<var>p1</var><sub>i16x8</sub>[𝑛] == <var>p2</var><sub>i16x8</sub>[𝑛])</code> <br/>&nbsp;<br/>&nbsp; |
| <code>2</code><br/>&nbsp;<br/>&nbsp; | <code>v128</code><br/>&nbsp;<br/>&nbsp; | <code>i32x4\.eq</code><br/><code>s32x4\.eq</code><br/><code>u32x4\.eq</code> | | <code><var>r</var><sub>i32x4</sub>[𝑛:0..3] ← (<var>p1</var><sub>i32x4</sub>[𝑛] == <var>p2</var><sub>i32x4</sub>[𝑛])</code> <br/>&nbsp;<br/>&nbsp; |
| <code>2</code><br/>&nbsp;<br/>&nbsp; | <code>v128</code><br/>&nbsp;<br/>&nbsp; | <code>i64x2\.eq</code><br/><code>s64x2\.eq</code><br/><code>u64x2\.eq</code> | | <code><var>r</var><sub>i64x2</sub>[𝑛:0..1] ← (<var>p1</var><sub>i64x2</sub>[𝑛] == <var>p2</var><sub>i64x2</sub>[𝑛])</code> <br/>&nbsp;<br/>&nbsp; |
| <code>2</code> | <code>v128</code> | <code>f32x4\.eq</code> | | <code><var>r</var><sub>i32x4</sub>[𝑛:0..3] ← (<var>p1</var><sub>f32x4</sub>[𝑛] == <var>p2</var><sub>f32x4</sub>[𝑛])</code> |
| <code>2</code> | <code>v128</code> | <code>f64x2\.eq</code> | | <code><var>r</var><sub>i64x2</sub>[𝑛:0..1] ← (<var>p1</var><sub>f64x2</sub>[𝑛] == <var>p2</var><sub>f64x2</sub>[𝑛])</code> |


### 条件：不一致

| 引数 | 返値 | 命令 | OP | 備考 |
|:-:|:-:|:--|:--|:--|
| <code>2</code><br/>&nbsp;<br/>&nbsp; | <code>i32</code><br/>&nbsp;<br/>&nbsp; | <code>i32\.ne</code><br/><code>s32\.ne</code><br/><code>u32\.ne</code> | | <code><var>r</var><sub>i32</sub> ← (<var>p1</var><sub>i32</sub> != <var>p2</var><sub>i32</sub>)</code> <br/>&nbsp;<br/>&nbsp; |
| <code>2</code><br/>&nbsp;<br/>&nbsp; | <code>i32</code><br/>&nbsp;<br/>&nbsp; | <code>i64\.ne</code><br/><code>s64\.ne</code><br/><code>u64\.ne</code> | | <code><var>r</var><sub>i32</sub> ← (<var>p1</var><sub>i64</sub> != <var>p2</var><sub>i64</sub>)</code> <br/>&nbsp;<br/>&nbsp; |
| <code>2</code> | <code>i32</code> | <code>f32\.ne</code> | | <code><var>r</var><sub>i32</sub> ← (<var>p1</var><sub>f32</sub> != <var>p2</var><sub>f32</sub>)</code> |
| <code>2</code> | <code>i32</code> | <code>f64\.ne</code> | | <code><var>r</var><sub>i32</sub> ← (<var>p1</var><sub>f64</sub> != <var>p2</var><sub>f64</sub>)</code> |
| | | | | |
| <code>2</code><br/>&nbsp;<br/>&nbsp; | <code>v128</code><br/>&nbsp;<br/>&nbsp; | <code>i8x16\.ne</code><br/><code>s8x16\.ne</code><br/><code>u8x16\.ne</code> | | <code><var>r</var><sub>i8x16</sub>[𝑛:0..15] ← (<var>p1</var><sub>i8x16</sub>[𝑛] != <var>p2</var><sub>i8x16</sub>[𝑛])</code> <br/>&nbsp;<br/>&nbsp; |
| <code>2</code><br/>&nbsp;<br/>&nbsp; | <code>v128</code><br/>&nbsp;<br/>&nbsp; | <code>i16x8\.ne</code><br/><code>s16x8\.ne</code><br/><code>u16x8\.ne</code> | | <code><var>r</var><sub>i16x8</sub>[𝑛:0..7] ← (<var>p1</var><sub>i16x8</sub>[𝑛] != <var>p2</var><sub>i16x8</sub>[𝑛])</code> <br/>&nbsp;<br/>&nbsp; |
| <code>2</code><br/>&nbsp;<br/>&nbsp; | <code>v128</code><br/>&nbsp;<br/>&nbsp; | <code>i32x4\.ne</code><br/><code>s32x4\.ne</code><br/><code>u32x4\.ne</code> | | <code><var>r</var><sub>i32x4</sub>[𝑛:0..3] ← (<var>p1</var><sub>i32x4</sub>[𝑛] != <var>p2</var><sub>i32x4</sub>[𝑛])</code> <br/>&nbsp;<br/>&nbsp; |
| <code>2</code><br/>&nbsp;<br/>&nbsp; | <code>v128</code><br/>&nbsp;<br/>&nbsp; | <code>i64x2\.ne</code><br/><code>s64x2\.ne</code><br/><code>u64x2\.ne</code> | | <code><var>r</var><sub>i64x2</sub>[𝑛:0..1] ← (<var>p1</var><sub>i64x2</sub>[𝑛] != <var>p2</var><sub>i64x2</sub>[𝑛])</code> <br/>&nbsp;<br/>&nbsp; |
| <code>2</code> | <code>v128</code> | <code>f32x4\.ne</code> | | <code><var>r</var><sub>i32x4</sub>[𝑛:0..3] ← (<var>p1</var><sub>f32x4</sub>[𝑛] != <var>p2</var><sub>f32x4</sub>[𝑛])</code> |
| <code>2</code> | <code>v128</code> | <code>f64x2\.ne</code> | | <code><var>r</var><sub>i64x2</sub>[𝑛:0..1] ← (<var>p1</var><sub>f64x2</sub>[𝑛] != <var>p2</var><sub>f64x2</sub>[𝑛])</code> |


### 条件：小なり

| 引数 | 返値 | 命令 | OP | 備考 |
|:-:|:-:|:--|:--|:--|
| <code>2</code><br/>&nbsp; | <code>i32</code><br/>&nbsp; | <code>i32\.lt\_s</code><br/><code>s32\.lt</code> | | <code><var>r</var><sub>i32</sub> ← (<var>p1</var><sub>s32</sub> &lt; <var>p2</var><sub>s32</sub>)</code> <br/>&nbsp; |
| <code>2</code><br/>&nbsp; | <code>i32</code><br/>&nbsp; | <code>i32\.lt\_u</code><br/><code>u32\.lt</code> | | <code><var>r</var><sub>i32</sub> ← (<var>p1</var><sub>u32</sub> &lt; <var>p2</var><sub>u32</sub>)</code> <br/>&nbsp; |
| <code>2</code><br/>&nbsp; | <code>i32</code><br/>&nbsp; | <code>i64\.lt\_s</code><br/><code>s64\.lt</code> | | <code><var>r</var><sub>i32</sub> ← (<var>p1</var><sub>s64</sub> &lt; <var>p2</var><sub>s64</sub>)</code> <br/>&nbsp; |
| <code>2</code><br/>&nbsp; | <code>i32</code><br/>&nbsp; | <code>i64\.lt\_u</code><br/><code>u64\.lt</code> | | <code><var>r</var><sub>i32</sub> ← (<var>p1</var><sub>u64</sub> &lt; <var>p2</var><sub>u64</sub>)</code> <br/>&nbsp; |
| <code>2</code> | <code>i32</code> | <code>f32\.lt</code> | | <code><var>r</var><sub>i32</sub> ← (<var>p1</var><sub>f32</sub> &lt; <var>p2</var><sub>f32</sub>)</code> |
| <code>2</code> | <code>i32</code> | <code>f64\.lt</code> | | <code><var>r</var><sub>i32</sub> ← (<var>p1</var><sub>f64</sub> &lt; <var>p2</var><sub>f64</sub>)</code> |
| | | | | |
| <code>2</code><br/>&nbsp; | <code>v128</code><br/>&nbsp; | <code>i8x16\.lt\_s</code><br/><code>s8x16\.lt</code> | | <code><var>r</var><sub>i8x16</sub>[𝑛:0..15] ← (<var>p1</var><sub>s8x16</sub>[𝑛] &lt; <var>p2</var><sub>s8x16</sub>[𝑛])</code> <br/>&nbsp; |
| <code>2</code><br/>&nbsp; | <code>v128</code><br/>&nbsp; | <code>i8x16\.lt\_u</code><br/><code>u8x16\.lt</code> | | <code><var>r</var><sub>i8x16</sub>[𝑛:0..15] ← (<var>p1</var><sub>u8x16</sub>[𝑛] &lt; <var>p2</var><sub>u8x16</sub>[𝑛])</code> <br/>&nbsp; |
| <code>2</code><br/>&nbsp; | <code>v128</code><br/>&nbsp; | <code>i16x8\.lt\_s</code><br/><code>s16x8\.lt</code> | | <code><var>r</var><sub>i16x8</sub>[𝑛:0..7] ← (<var>p1</var><sub>s16x8</sub>[𝑛] &lt; <var>p2</var><sub>s16x8</sub>[𝑛])</code> <br/>&nbsp; |
| <code>2</code><br/>&nbsp; | <code>v128</code><br/>&nbsp; | <code>i16x8\.lt\_u</code><br/><code>u16x8\.lt</code> | | <code><var>r</var><sub>i16x8</sub>[𝑛:0..7] ← (<var>p1</var><sub>u16x8</sub>[𝑛] &lt; <var>p2</var><sub>u16x8</sub>[𝑛])</code> <br/>&nbsp; |
| <code>2</code><br/>&nbsp; | <code>v128</code><br/>&nbsp; | <code>i32x4\.lt\_s</code><br/><code>s32x4\.lt</code> | | <code><var>r</var><sub>i32x4</sub>[𝑛:0..3] ← (<var>p1</var><sub>s32x4</sub>[𝑛] &lt; <var>p2</var><sub>s32x4</sub>[𝑛])</code> <br/>&nbsp; |
| <code>2</code><br/>&nbsp; | <code>v128</code><br/>&nbsp; | <code>i32x4\.lt\_u</code><br/><code>u32x4\.lt</code> | | <code><var>r</var><sub>i32x4</sub>[𝑛:0..3] ← (<var>p1</var><sub>u32x4</sub>[𝑛] &lt; <var>p2</var><sub>u32x4</sub>[𝑛])</code> <br/>&nbsp; |
| <code>2</code><br/>&nbsp; | <code>v128</code><br/>&nbsp; | <code>i64x2\.lt\_s</code><br/><code>s64x2\.lt</code> | | <code><var>r</var><sub>i64x2</sub>[𝑛:0..1] ← (<var>p1</var><sub>s64x2</sub>[𝑛] &lt; <var>p2</var><sub>s64x2</sub>[𝑛])</code> <br/>&nbsp; |
| <code>2</code> | <code>v128</code> | <code>f32x4\.lt</code> | | <code><var>r</var><sub>i32x4</sub>[𝑛:0..3] ← (<var>p1</var><sub>f32x4</sub>[𝑛] &lt; <var>p2</var><sub>f32x4</sub>[𝑛])</code> |
| <code>2</code> | <code>v128</code> | <code>f64x2\.lt</code> | | <code><var>r</var><sub>i64x2</sub>[𝑛:0..1] ← (<var>p1</var><sub>f64x2</sub>[𝑛] &lt; <var>p2</var><sub>f64x2</sub>[𝑛])</code> |


### 条件：以下

| 引数 | 返値 | 命令 | OP | 備考 |
|:-:|:-:|:--|:--|:--|
| <code>2</code><br/>&nbsp; | <code>i32</code><br/>&nbsp; | <code>i32\.le\_s</code><br/><code>s32\.le</code> | | <code><var>r</var><sub>i32</sub> ← (<var>p1</var><sub>s32</sub> &lt;= <var>p2</var><sub>s32</sub>)</code> <br/>&nbsp; |
| <code>2</code><br/>&nbsp; | <code>i32</code><br/>&nbsp; | <code>i32\.le\_u</code><br/><code>u32\.le</code> | | <code><var>r</var><sub>i32</sub> ← (<var>p1</var><sub>u32</sub> &lt;= <var>p2</var><sub>u32</sub>)</code> <br/>&nbsp; |
| <code>2</code><br/>&nbsp; | <code>i32</code><br/>&nbsp; | <code>i64\.le\_s</code><br/><code>s64\.le</code> | | <code><var>r</var><sub>i32</sub> ← (<var>p1</var><sub>s64</sub> &lt;= <var>p2</var><sub>s64</sub>)</code> <br/>&nbsp; |
| <code>2</code><br/>&nbsp; | <code>i32</code><br/>&nbsp; | <code>i64\.le\_u</code><br/><code>u64\.le</code> | | <code><var>r</var><sub>i32</sub> ← (<var>p1</var><sub>u64</sub> &lt;= <var>p2</var><sub>u64</sub>)</code> <br/>&nbsp; |
| <code>2</code> | <code>i32</code> | <code>f32\.le</code> | | <code><var>r</var><sub>i32</sub> ← (<var>p1</var><sub>f32</sub> &lt;= <var>p2</var><sub>f32</sub>)</code> |
| <code>2</code> | <code>i32</code> | <code>f64\.le</code> | | <code><var>r</var><sub>i32</sub> ← (<var>p1</var><sub>f64</sub> &lt;= <var>p2</var><sub>f64</sub>)</code> |
| | | | | |
| <code>2</code><br/>&nbsp; | <code>v128</code><br/>&nbsp; | <code>i8x16\.le\_s</code><br/><code>s8x16\.le</code> | | <code><var>r</var><sub>i8x16</sub>[𝑛:0..15] ← (<var>p1</var><sub>s8x16</sub>[𝑛] &lt;= <var>p2</var><sub>s8x16</sub>[𝑛])</code> <br/>&nbsp; |
| <code>2</code><br/>&nbsp; | <code>v128</code><br/>&nbsp; | <code>i8x16\.le\_u</code><br/><code>u8x16\.le</code> | | <code><var>r</var><sub>i8x16</sub>[𝑛:0..15] ← (<var>p1</var><sub>u8x16</sub>[𝑛] &lt;= <var>p2</var><sub>u8x16</sub>[𝑛])</code> <br/>&nbsp; |
| <code>2</code><br/>&nbsp; | <code>v128</code><br/>&nbsp; | <code>i16x8\.le\_s</code><br/><code>s16x8\.le</code> | | <code><var>r</var><sub>i16x8</sub>[𝑛:0..7] ← (<var>p1</var><sub>s16x8</sub>[𝑛] &lt;= <var>p2</var><sub>s16x8</sub>[𝑛])</code> <br/>&nbsp; |
| <code>2</code><br/>&nbsp; | <code>v128</code><br/>&nbsp; | <code>i16x8\.le\_u</code><br/><code>u16x8\.le</code> | | <code><var>r</var><sub>i16x8</sub>[𝑛:0..7] ← (<var>p1</var><sub>u16x8</sub>[𝑛] &lt;= <var>p2</var><sub>u16x8</sub>[𝑛])</code> <br/>&nbsp; |
| <code>2</code><br/>&nbsp; | <code>v128</code><br/>&nbsp; | <code>i32x4\.le\_s</code><br/><code>s32x4\.le</code> | | <code><var>r</var><sub>i32x4</sub>[𝑛:0..3] ← (<var>p1</var><sub>s32x4</sub>[𝑛] &lt;= <var>p2</var><sub>s32x4</sub>[𝑛])</code> <br/>&nbsp; |
| <code>2</code><br/>&nbsp; | <code>v128</code><br/>&nbsp; | <code>i32x4\.le\_u</code><br/><code>u32x4\.le</code> | | <code><var>r</var><sub>i32x4</sub>[𝑛:0..3] ← (<var>p1</var><sub>u32x4</sub>[𝑛] &lt;= <var>p2</var><sub>u32x4</sub>[𝑛])</code> <br/>&nbsp; |
| <code>2</code><br/>&nbsp; | <code>v128</code><br/>&nbsp; | <code>i64x2\.le\_s</code><br/><code>s64x2\.le</code> | | <code><var>r</var><sub>i64x2</sub>[𝑛:0..1] ← (<var>p1</var><sub>s64x2</sub>[𝑛] &lt;= <var>p2</var><sub>s64x2</sub>[𝑛])</code> <br/>&nbsp; |
| <code>2</code> | <code>v128</code> | <code>f32x4\.le</code> | | <code><var>r</var><sub>i32x4</sub>[𝑛:0..3] ← (<var>p1</var><sub>f32x4</sub>[𝑛] &lt;= <var>p2</var><sub>f32x4</sub>[𝑛])</code> |
| <code>2</code> | <code>v128</code> | <code>f64x2\.le</code> | | <code><var>r</var><sub>i64x2</sub>[𝑛:0..1] ← (<var>p1</var><sub>f64x2</sub>[𝑛] &lt;= <var>p2</var><sub>f64x2</sub>[𝑛])</code> |


### 条件：以上

| 引数 | 返値 | 命令 | OP | 備考 |
|:-:|:-:|:--|:--|:--|
| <code>2</code><br/>&nbsp; | <code>i32</code><br/>&nbsp; | <code>i32\.ge\_s</code><br/><code>s32\.ge</code> | | <code><var>r</var><sub>i32</sub> ← (<var>p1</var><sub>s32</sub> &gt;= <var>p2</var><sub>s32</sub>)</code> <br/>&nbsp; |
| <code>2</code><br/>&nbsp; | <code>i32</code><br/>&nbsp; | <code>i32\.ge\_u</code><br/><code>u32\.ge</code> | | <code><var>r</var><sub>i32</sub> ← (<var>p1</var><sub>u32</sub> &gt;= <var>p2</var><sub>u32</sub>)</code> <br/>&nbsp; |
| <code>2</code><br/>&nbsp; | <code>i32</code><br/>&nbsp; | <code>i64\.ge\_s</code><br/><code>s64\.ge</code> | | <code><var>r</var><sub>i32</sub> ← (<var>p1</var><sub>s64</sub> &gt;= <var>p2</var><sub>s64</sub>)</code> <br/>&nbsp; |
| <code>2</code><br/>&nbsp; | <code>i32</code><br/>&nbsp; | <code>i64\.ge\_u</code><br/><code>u64\.ge</code> | | <code><var>r</var><sub>i32</sub> ← (<var>p1</var><sub>u64</sub> &gt;= <var>p2</var><sub>u64</sub>)</code> <br/>&nbsp; |
| <code>2</code> | <code>i32</code> | <code>f32\.ge</code> | | <code><var>r</var><sub>i32</sub> ← (<var>p1</var><sub>f32</sub> &gt;= <var>p2</var><sub>f32</sub>)</code> |
| <code>2</code> | <code>i32</code> | <code>f64\.ge</code> | | <code><var>r</var><sub>i32</sub> ← (<var>p1</var><sub>f64</sub> &gt;= <var>p2</var><sub>f64</sub>)</code> |
| | | | | |
| <code>2</code><br/>&nbsp; | <code>v128</code><br/>&nbsp; | <code>i8x16\.ge\_s</code><br/><code>s8x16\.ge</code> | | <code><var>r</var><sub>i8x16</sub>[𝑛:0..15] ← (<var>p1</var><sub>s8x16</sub>[𝑛] &gt;= <var>p2</var><sub>s8x16</sub>[𝑛])</code> <br/>&nbsp; |
| <code>2</code><br/>&nbsp; | <code>v128</code><br/>&nbsp; | <code>i8x16\.ge\_u</code><br/><code>u8x16\.ge</code> | | <code><var>r</var><sub>i8x16</sub>[𝑛:0..15] ← (<var>p1</var><sub>u8x16</sub>[𝑛] &gt;= <var>p2</var><sub>u8x16</sub>[𝑛])</code> <br/>&nbsp; |
| <code>2</code><br/>&nbsp; | <code>v128</code><br/>&nbsp; | <code>i16x8\.ge\_s</code><br/><code>s16x8\.ge</code> | | <code><var>r</var><sub>i16x8</sub>[𝑛:0..7] ← (<var>p1</var><sub>s16x8</sub>[𝑛] &gt;= <var>p2</var><sub>s16x8</sub>[𝑛])</code> <br/>&nbsp; |
| <code>2</code><br/>&nbsp; | <code>v128</code><br/>&nbsp; | <code>i16x8\.ge\_u</code><br/><code>u16x8\.ge</code> | | <code><var>r</var><sub>i16x8</sub>[𝑛:0..7] ← (<var>p1</var><sub>u16x8</sub>[𝑛] &gt;= <var>p2</var><sub>u16x8</sub>[𝑛])</code> <br/>&nbsp; |
| <code>2</code><br/>&nbsp; | <code>v128</code><br/>&nbsp; | <code>i32x4\.ge\_s</code><br/><code>s32x4\.ge</code> | | <code><var>r</var><sub>i32x4</sub>[𝑛:0..3] ← (<var>p1</var><sub>s32x4</sub>[𝑛] &gt;= <var>p2</var><sub>s32x4</sub>[𝑛])</code> <br/>&nbsp; |
| <code>2</code><br/>&nbsp; | <code>v128</code><br/>&nbsp; | <code>i32x4\.ge\_u</code><br/><code>u32x4\.ge</code> | | <code><var>r</var><sub>i32x4</sub>[𝑛:0..3] ← (<var>p1</var><sub>u32x4</sub>[𝑛] &gt;= <var>p2</var><sub>u32x4</sub>[𝑛])</code> <br/>&nbsp; |
| <code>2</code><br/>&nbsp; | <code>v128</code><br/>&nbsp; | <code>i64x2\.ge\_s</code><br/><code>s64x2\.ge</code> | | <code><var>r</var><sub>i64x2</sub>[𝑛:0..1] ← (<var>p1</var><sub>s64x2</sub>[𝑛] &gt;= <var>p2</var><sub>s64x2</sub>[𝑛])</code> <br/>&nbsp; |
| <code>2</code> | <code>v128</code> | <code>f32x4\.ge</code> | | <code><var>r</var><sub>i32x4</sub>[𝑛:0..3] ← (<var>p1</var><sub>f32x4</sub>[𝑛] &gt;= <var>p2</var><sub>f32x4</sub>[𝑛])</code> |
| <code>2</code> | <code>v128</code> | <code>f64x2\.ge</code> | | <code><var>r</var><sub>i64x2</sub>[𝑛:0..1] ← (<var>p1</var><sub>f64x2</sub>[𝑛] &gt;= <var>p2</var><sub>f64x2</sub>[𝑛])</code> |


### 条件：大なり

| 引数 | 返値 | 命令 | OP | 備考 |
|:-:|:-:|:--|:--|:--|
| <code>2</code><br/>&nbsp; | <code>i32</code><br/>&nbsp; | <code>i32\.gt\_s</code><br/><code>s32\.gt</code> | | <code><var>r</var><sub>i32</sub> ← (<var>p1</var><sub>s32</sub> &gt; <var>p2</var><sub>s32</sub>)</code> <br/>&nbsp; |
| <code>2</code><br/>&nbsp; | <code>i32</code><br/>&nbsp; | <code>i32\.gt\_u</code><br/><code>u32\.gt</code> | | <code><var>r</var><sub>i32</sub> ← (<var>p1</var><sub>u32</sub> &gt; <var>p2</var><sub>u32</sub>)</code> <br/>&nbsp; |
| <code>2</code><br/>&nbsp; | <code>i32</code><br/>&nbsp; | <code>i64\.gt\_s</code><br/><code>s64\.gt</code> | | <code><var>r</var><sub>i32</sub> ← (<var>p1</var><sub>s64</sub> &gt; <var>p2</var><sub>s64</sub>)</code> <br/>&nbsp; |
| <code>2</code><br/>&nbsp; | <code>i32</code><br/>&nbsp; | <code>i64\.gt\_u</code><br/><code>u64\.gt</code> | | <code><var>r</var><sub>i32</sub> ← (<var>p1</var><sub>u64</sub> &gt; <var>p2</var><sub>u64</sub>)</code> <br/>&nbsp; |
| <code>2</code> | <code>i32</code> | <code>f32\.gt</code> | | <code><var>r</var><sub>i32</sub> ← (<var>p1</var><sub>f32</sub> &gt; <var>p2</var><sub>f32</sub>)</code> |
| <code>2</code> | <code>i32</code> | <code>f64\.gt</code> | | <code><var>r</var><sub>i32</sub> ← (<var>p1</var><sub>f64</sub> &gt; <var>p2</var><sub>f64</sub>)</code> |
| | | | | |
| <code>2</code><br/>&nbsp; | <code>v128</code><br/>&nbsp; | <code>i8x16\.gt\_s</code><br/><code>s8x16\.gt</code> | | <code><var>r</var><sub>i8x16</sub>[𝑛:0..15] ← (<var>p1</var><sub>s8x16</sub>[𝑛] &gt; <var>p2</var><sub>s8x16</sub>[𝑛])</code> <br/>&nbsp; |
| <code>2</code><br/>&nbsp; | <code>v128</code><br/>&nbsp; | <code>i8x16\.gt\_u</code><br/><code>u8x16\.gt</code> | | <code><var>r</var><sub>i8x16</sub>[𝑛:0..15] ← (<var>p1</var><sub>u8x16</sub>[𝑛] &gt; <var>p2</var><sub>u8x16</sub>[𝑛])</code> <br/>&nbsp; |
| <code>2</code><br/>&nbsp; | <code>v128</code><br/>&nbsp; | <code>i16x8\.gt\_s</code><br/><code>s16x8\.gt</code> | | <code><var>r</var><sub>i16x8</sub>[𝑛:0..7] ← (<var>p1</var><sub>s16x8</sub>[𝑛] &gt; <var>p2</var><sub>s16x8</sub>[𝑛])</code> <br/>&nbsp; |
| <code>2</code><br/>&nbsp; | <code>v128</code><br/>&nbsp; | <code>i16x8\.gt\_u</code><br/><code>u16x8\.gt</code> | | <code><var>r</var><sub>i16x8</sub>[𝑛:0..7] ← (<var>p1</var><sub>u16x8</sub>[𝑛] &gt; <var>p2</var><sub>u16x8</sub>[𝑛])</code> <br/>&nbsp; |
| <code>2</code><br/>&nbsp; | <code>v128</code><br/>&nbsp; | <code>i32x4\.gt\_s</code><br/><code>s32x4\.gt</code> | | <code><var>r</var><sub>i32x4</sub>[𝑛:0..3] ← (<var>p1</var><sub>s32x4</sub>[𝑛] &gt; <var>p2</var><sub>s32x4</sub>[𝑛])</code> <br/>&nbsp; |
| <code>2</code><br/>&nbsp; | <code>v128</code><br/>&nbsp; | <code>i32x4\.gt\_u</code><br/><code>u32x4\.gt</code> | | <code><var>r</var><sub>i32x4</sub>[𝑛:0..3] ← (<var>p1</var><sub>u32x4</sub>[𝑛] &gt; <var>p2</var><sub>u32x4</sub>[𝑛])</code> <br/>&nbsp; |
| <code>2</code><br/>&nbsp; | <code>v128</code><br/>&nbsp; | <code>i64x2\.gt\_s</code><br/><code>s64x2\.gt</code> | | <code><var>r</var><sub>i64x2</sub>[𝑛:0..1] ← (<var>p1</var><sub>s64x2</sub>[𝑛] &gt; <var>p2</var><sub>s64x2</sub>[𝑛])</code> <br/>&nbsp; |
| <code>2</code> | <code>v128</code> | <code>f32x4\.gt</code> | | <code><var>r</var><sub>i32x4</sub>[𝑛:0..3] ← (<var>p1</var><sub>f32x4</sub>[𝑛] &gt; <var>p2</var><sub>f32x4</sub>[𝑛])</code> |
| <code>2</code> | <code>v128</code> | <code>f64x2\.gt</code> | | <code><var>r</var><sub>i64x2</sub>[𝑛:0..1] ← (<var>p1</var><sub>f64x2</sub>[𝑛] &gt; <var>p2</var><sub>f64x2</sub>[𝑛])</code> |


### 条件：全要素

| 引数 | 返値 | 命令 | OP | 備考 |
|:-:|:-:|:--|:--|:--|
| <code>1</code><br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp; | <code>i32</code><br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp; | <code>v128\.any\_true</code><br/><code>i8x16\.any\_true</code><br/><code>s8x16\.any\_true</code><br/><code>u8x16\.any\_true</code><br/><code>i16x8\.any\_true</code><br/><code>s16x8\.any\_true</code><br/><code>u16x8\.any\_true</code><br/><code>i32x4\.any\_true</code><br/><code>s32x4\.any\_true</code><br/><code>u32x4\.any\_true</code><br/><code>i64x2\.any\_true</code><br/><code>s64x2\.any\_true</code><br/><code>u64x2\.any\_true</code><br/><code>f32x4\.any\_true</code><br/><code>f64x2\.any\_true</code> | | <code><var>r</var><sub>i32</sub> ← (<var>p1</var><sub>i128</sub> != 0)</code> <br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp; |
| <code>1</code><br/>&nbsp;<br/>&nbsp; | <code>i32</code><br/>&nbsp;<br/>&nbsp; | <code>i8x16\.all\_true</code><br/><code>s8x16\.all\_true</code><br/><code>u8x16\.all\_true</code> | | <code><var>r</var><sub>i32</sub> ← (<var>p1</var><sub>i8x16</sub>[0] &amp;&amp; ... &amp;&amp; <var>p1</var><sub>i8x16</sub>[15])</code> <br/>&nbsp;<br/>&nbsp; |
| <code>1</code><br/>&nbsp;<br/>&nbsp; | <code>i32</code><br/>&nbsp;<br/>&nbsp; | <code>i16x8\.all\_true</code><br/><code>s16x8\.all\_true</code><br/><code>u16x8\.all\_true</code> | | <code><var>r</var><sub>i32</sub> ← (<var>p1</var><sub>i16x8</sub>[0] &amp;&amp; ... &amp;&amp; <var>p1</var><sub>i16x8</sub>[7])</code> <br/>&nbsp;<br/>&nbsp; |
| <code>1</code><br/>&nbsp;<br/>&nbsp;<br/>&nbsp; | <code>i32</code><br/>&nbsp;<br/>&nbsp;<br/>&nbsp; | <code>i32x4\.all\_true</code><br/><code>s32x4\.all\_true</code><br/><code>u32x4\.all\_true</code><br/><code>f32x4\.all\_true</code> | | <code><var>r</var><sub>i32</sub> ← (<var>p1</var><sub>i32x4</sub>[0] &amp;&amp; ... &amp;&amp; <var>p1</var><sub>i32x4</sub>[3])</code> <br/>&nbsp;<br/>&nbsp;<br/>&nbsp; |
| <code>1</code><br/>&nbsp;<br/>&nbsp;<br/>&nbsp; | <code>i32</code><br/>&nbsp;<br/>&nbsp;<br/>&nbsp; | <code>i64x2\.all\_true</code><br/><code>s64x2\.all\_true</code><br/><code>u64x2\.all\_true</code><br/><code>f64x2\.all\_true</code> | | <code><var>r</var><sub>i32</sub> ← (<var>p1</var><sub>i64x2</sub>[0] &amp;&amp; <var>p1</var><sub>i64x2</sub>[1])</code> <br/>&nbsp;<br/>&nbsp;<br/>&nbsp; |


## 論理演算

| 引数 | 返値 | 命令 | OP | 備考 |
|:-:|:-:|:--|:--|:--|
| <code>2</code><br/>&nbsp;<br/>&nbsp; | <code>i32</code><br/>&nbsp;<br/>&nbsp; | <code>i32\.and</code><br/><code>s32\.and</code><br/><code>u32\.and</code> | | <code><var>r</var><sub>i32</sub> ← <var>p1</var><sub>i32</sub> &amp; <var>p2</var><sub>i32</sub></code> <br/>&nbsp;<br/>&nbsp; |
| <code>2</code><br/>&nbsp;<br/>&nbsp; | <code>i64</code><br/>&nbsp;<br/>&nbsp; | <code>i64\.and</code><br/><code>s64\.and</code><br/><code>u64\.and</code> | | <code><var>r</var><sub>i64</sub> ← <var>p1</var><sub>i64</sub> &amp; <var>p2</var><sub>i64</sub></code> <br/>&nbsp;<br/>&nbsp; |
| <code>2</code><br/>&nbsp;<br/>&nbsp; | <code>i32</code><br/>&nbsp;<br/>&nbsp; | <code>i32\.or</code><br/><code>s32\.or</code><br/><code>u32\.or</code> | | <code><var>r</var><sub>i32</sub> ← <var>p1</var><sub>i32</sub> \| <var>p2</var><sub>i32</sub></code> <br/>&nbsp;<br/>&nbsp; |
| <code>2</code><br/>&nbsp;<br/>&nbsp; | <code>i64</code><br/>&nbsp;<br/>&nbsp; | <code>i64\.or</code><br/><code>s64\.or</code><br/><code>u64\.or</code> | | <code><var>r</var><sub>i64</sub> ← <var>p1</var><sub>i64</sub> \| <var>p2</var><sub>i64</sub></code> <br/>&nbsp;<br/>&nbsp; |
| <code>2</code><br/>&nbsp;<br/>&nbsp; | <code>i32</code><br/>&nbsp;<br/>&nbsp; | <code>i32\.xor</code><br/><code>s32\.xor</code><br/><code>u32\.xor</code> | | <code><var>r</var><sub>i32</sub> ← <var>p1</var><sub>i32</sub> ^ <var>p2</var><sub>i32</sub></code> <br/>&nbsp;<br/>&nbsp; |
| <code>2</code><br/>&nbsp;<br/>&nbsp; | <code>i64</code><br/>&nbsp;<br/>&nbsp; | <code>i64\.xor</code><br/><code>s64\.xor</code><br/><code>u64\.xor</code> | | <code><var>r</var><sub>i64</sub> ← <var>p1</var><sub>i64</sub> ^ <var>p2</var><sub>i64</sub></code> <br/>&nbsp;<br/>&nbsp; |
| | | | | |
| <code>1</code><br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp; | <code>v128</code><br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp; | <code>v128\.not</code><br/><code>i8x16\.not</code><br/><code>s8x16\.not</code><br/><code>u8x16\.not</code><br/><code>i16x8\.not</code><br/><code>s16x8\.not</code><br/><code>u16x8\.not</code><br/><code>i32x4\.not</code><br/><code>s32x4\.not</code><br/><code>u32x4\.not</code><br/><code>i64x2\.not</code><br/><code>s64x2\.not</code><br/><code>u64x2\.not</code> | | <code><var>r</var><sub>v128</sub> ← ~ <var>p1</var><sub>v128</sub></code> <br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp; |
| <code>2</code><br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp; | <code>v128</code><br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp; | <code>v128\.and</code><br/><code>i8x16\.and</code><br/><code>s8x16\.and</code><br/><code>u8x16\.and</code><br/><code>i16x8\.and</code><br/><code>s16x8\.and</code><br/><code>u16x8\.and</code><br/><code>i32x4\.and</code><br/><code>s32x4\.and</code><br/><code>u32x4\.and</code><br/><code>i64x2\.and</code><br/><code>s64x2\.and</code><br/><code>u64x2\.and</code> | | <code><var>r</var><sub>v128</sub> ← <var>p1</var><sub>v128</sub> &amp; <var>p2</var><sub>v128</sub></code> <br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp; |
| <code>2</code><br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp; | <code>v128</code><br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp; | <code>v128\.andnot</code><br/><code>i8x16\.andnot</code><br/><code>s8x16\.andnot</code><br/><code>u8x16\.andnot</code><br/><code>i16x8\.andnot</code><br/><code>s16x8\.andnot</code><br/><code>u16x8\.andnot</code><br/><code>i32x4\.andnot</code><br/><code>s32x4\.andnot</code><br/><code>u32x4\.andnot</code><br/><code>i64x2\.andnot</code><br/><code>s64x2\.andnot</code><br/><code>u64x2\.andnot</code> | | <code><var>r</var><sub>v128</sub> ← <var>p1</var><sub>v128</sub> &amp; ~<var>p2</var><sub>v128</sub></code> <br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp; |
| <code>2</code><br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp; | <code>v128</code><br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp; | <code>v128\.or</code><br/><code>i8x16\.or</code><br/><code>s8x16\.or</code><br/><code>u8x16\.or</code><br/><code>i16x8\.or</code><br/><code>s16x8\.or</code><br/><code>u16x8\.or</code><br/><code>i32x4\.or</code><br/><code>s32x4\.or</code><br/><code>u32x4\.or</code><br/><code>i64x2\.or</code><br/><code>s64x2\.or</code><br/><code>u64x2\.or</code> | | <code><var>r</var><sub>v128</sub> ← <var>p1</var><sub>v128</sub> \| <var>p2</var><sub>v128</sub></code> <br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp; |
| <code>2</code><br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp; | <code>v128</code><br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp; | <code>v128\.xor</code><br/><code>i8x16\.xor</code><br/><code>s8x16\.xor</code><br/><code>u8x16\.xor</code><br/><code>i16x8\.xor</code><br/><code>s16x8\.xor</code><br/><code>u16x8\.xor</code><br/><code>i32x4\.xor</code><br/><code>s32x4\.xor</code><br/><code>u32x4\.xor</code><br/><code>i64x2\.xor</code><br/><code>s64x2\.xor</code><br/><code>u64x2\.xor</code> | | <code><var>r</var><sub>v128</sub> ← <var>p1</var><sub>v128</sub> ^ <var>p2</var><sub>v128</sub></code> <br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp; |
| <code>3</code><br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp; | <code>v128</code><br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp; | <code>v128\.bitselect</code><br/><code>i8x16\.bitselect</code><br/><code>s8x16\.bitselect</code><br/><code>u8x16\.bitselect</code><br/><code>i16x8\.bitselect</code><br/><code>s16x8\.bitselect</code><br/><code>u16x8\.bitselect</code><br/><code>i32x4\.bitselect</code><br/><code>s32x4\.bitselect</code><br/><code>u32x4\.bitselect</code><br/><code>i64x2\.bitselect</code><br/><code>s64x2\.bitselect</code><br/><code>u64x2\.bitselect</code> | | <code><var>r</var><sub>v128</sub> ← (<var>p1</var><sub>v128</sub> &amp; <var>p3</var><sub>v128</sub>) \| (<var>p2</var><sub>v128</sub> & ~<var>p3</var><sub>v128</sub>)</code> <br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp;<br/>&nbsp; |


## 算術演算


### 整数値化

| 引数 | 返値 | 命令 | OP | 備考 |
|:-:|:-:|:--|:--|:--|
| <code>1</code> | <code>f32</code> | <code>f32\.ceil</code> | | <code><var>r</var><sub>f32</sub> ← ceil(<var>p1</var><sub>f32</sub>)</code> |
| <code>1</code> | <code>f64</code> | <code>f64\.ceil</code> | | <code><var>r</var><sub>f64</sub> ← ceil(<var>p1</var><sub>f64</sub>)</code> |
| <code>1</code> | <code>f32</code> | <code>f32\.floor</code> | | <code><var>r</var><sub>f32</sub> ← floor(<var>p1</var><sub>f32</sub>)</code> |
| <code>1</code> | <code>f64</code> | <code>f64\.floor</code> | | <code><var>r</var><sub>f64</sub> ← floor(<var>p1</var><sub>f64</sub>)</code> |
| <code>1</code> | <code>f32</code> | <code>f32\.trunc</code> | | <code><var>r</var><sub>f32</sub> ← trunc(<var>p1</var><sub>f32</sub>)</code> |
| <code>1</code> | <code>f64</code> | <code>f64\.trunc</code> | | <code><var>r</var><sub>f64</sub> ← trunc(<var>p1</var><sub>f64</sub>)</code> |
| <code>1</code> | <code>f32</code> | <code>f32\.nearest</code> | | <code><var>r</var><sub>f32</sub> ← nearest(<var>p1</var><sub>f32</sub>)</code> |
| <code>1</code> | <code>f64</code> | <code>f64\.nearest</code> | | <code><var>r</var><sub>f64</sub> ← nearest(<var>p1</var><sub>f64</sub>)</code> |
| | | | | |
| <code>1</code> | <code>v128</code> | <code>f32x4\.ceil</code> | | <code><var>r</var><sub>f32x4</sub>[𝑛:0..3] ← ceil(<var>p1</var><sub>f32x4</sub>[𝑛])</code> |
| <code>1</code> | <code>v128</code> | <code>f64x2\.ceil</code> | | <code><var>r</var><sub>f64x2</sub>[𝑛:0..1] ← ceil(<var>p1</var><sub>f64x2</sub>[𝑛])</code> |
| <code>1</code> | <code>v128</code> | <code>f32x4\.floor</code> | | <code><var>r</var><sub>f32x4</sub>[𝑛:0..3] ← floor(<var>p1</var><sub>f32x4</sub>[𝑛])</code> |
| <code>1</code> | <code>v128</code> | <code>f64x2\.floor</code> | | <code><var>r</var><sub>f64x2</sub>[𝑛:0..1] ← floor(<var>p1</var><sub>f64x2</sub>[𝑛])</code> |
| <code>1</code> | <code>v128</code> | <code>f32x4\.trunc</code> | | <code><var>r</var><sub>f32x4</sub>[𝑛:0..3] ← trunc(<var>p1</var><sub>f32x4</sub>[𝑛])</code> |
| <code>1</code> | <code>v128</code> | <code>f64x2\.trunc</code> | | <code><var>r</var><sub>f64x2</sub>[𝑛:0..1] ← trunc(<var>p1</var><sub>f64x2</sub>[𝑛])</code> |
| <code>1</code> | <code>v128</code> | <code>f32x4\.nearest</code> | | <code><var>r</var><sub>f32x4</sub>[𝑛:0..3] ← nearest(<var>p1</var><sub>f32x4</sub>[𝑛])</code> |
| <code>1</code> | <code>v128</code> | <code>f64x2\.nearest</code> | | <code><var>r</var><sub>f64x2</sub>[𝑛:0..1] ← nearest(<var>p1</var><sub>f64x2</sub>[𝑛])</code> |


### 絶対値

| 引数 | 返値 | 命令 | OP | 備考 |
|:-:|:-:|:--|:--|:--|
| <code>1</code> | <code>f32</code> | <code>f32\.abs</code> | | <code><var>r</var><sub>f32</sub> ← abs(<var>p1</var><sub>f32</sub>)</code> |
| <code>1</code> | <code>f64</code> | <code>f64\.abs</code> | | <code><var>r</var><sub>f64</sub> ← abs(<var>p1</var><sub>f64</sub>)</code> |
| | | | | |
| <code>1</code><br/>&nbsp; | <code>v128</code><br/>&nbsp; | <code>i8x16\.abs</code><br/><code>s8x16\.abs</code> | | <code><var>r</var><sub>i8x16</sub>[𝑛:0..15] ← abs(<var>p1</var><sub>s8x16</sub>[𝑛])</code> <br/>&nbsp; |
| <code>1</code><br/>&nbsp; | <code>v128</code><br/>&nbsp; | <code>i16x8\.abs</code><br/><code>s16x8\.abs</code> | | <code><var>r</var><sub>i16x8</sub>[𝑛:0..7] ← abs(<var>p1</var><sub>s16x8</sub>[𝑛])</code> <br/>&nbsp; |
| <code>1</code><br/>&nbsp; | <code>v128</code><br/>&nbsp; | <code>i32x4\.abs</code><br/><code>s32x4\.abs</code> | | <code><var>r</var><sub>i32x4</sub>[𝑛:0..3] ← abs(<var>p1</var><sub>s32x4</sub>[𝑛])</code> <br/>&nbsp; |
| <code>1</code><br/>&nbsp; | <code>v128</code><br/>&nbsp; | <code>i64x2\.abs</code><br/><code>s64x2\.abs</code> | | <code><var>r</var><sub>i64x2</sub>[𝑛:0..1] ← abs(<var>p1</var><sub>s64x2</sub>[𝑛])</code> <br/>&nbsp; |
| <code>1</code> | <code>v128</code> | <code>f32x4\.abs</code> | | <code><var>r</var><sub>f32x4</sub>[𝑛:0..3] ← abs(<var>p1</var><sub>f32x4</sub>[𝑛])</code> |
| <code>1</code> | <code>v128</code> | <code>f64x2\.abs</code> | | <code><var>r</var><sub>f64x2</sub>[𝑛:0..1] ← abs(<var>p1</var><sub>f64x2</sub>[𝑛])</code> |


### 符号反転

| 引数 | 返値 | 命令 | OP | 備考 |
|:-:|:-:|:--|:--|:--|
| <code>1</code> | <code>f32</code> | <code>f32\.neg</code> | | <code><var>r</var><sub>f32</sub> ← -<var>p1</var><sub>f32</sub></code> |
| <code>1</code> | <code>f64</code> | <code>f64\.neg</code> | | <code><var>r</var><sub>f64</sub> ← -<var>p1</var><sub>f64</sub></code> |
| | | | | |
| <code>1</code><br/>&nbsp;<br/>&nbsp; | <code>v128</code><br/>&nbsp;<br/>&nbsp; | <code>i8x16\.neg</code><br/><code>s8x16\.neg</code><br/><code>u8x16\.neg</code> | | <code><var>r</var><sub>i8x16</sub>[𝑛:0..15] ← -<var>p1</var><sub>i8x16</sub>[𝑛]</code> <br/>&nbsp;<br/>&nbsp; |
| <code>1</code><br/>&nbsp;<br/>&nbsp; | <code>v128</code><br/>&nbsp;<br/>&nbsp; | <code>i16x8\.neg</code><br/><code>s16x8\.neg</code><br/><code>u16x8\.neg</code> | | <code><var>r</var><sub>i16x8</sub>[𝑛:0..7] ← -<var>p1</var><sub>i16x8</sub>[𝑛]</code> <br/>&nbsp;<br/>&nbsp; |
| <code>1</code><br/>&nbsp;<br/>&nbsp; | <code>v128</code><br/>&nbsp;<br/>&nbsp; | <code>i32x4\.neg</code><br/><code>s32x4\.neg</code><br/><code>u32x4\.neg</code> | | <code><var>r</var><sub>i32x4</sub>[𝑛:0..3] ← -<var>p1</var><sub>i32x4</sub>[𝑛]</code> <br/>&nbsp;<br/>&nbsp; |
| <code>1</code><br/>&nbsp;<br/>&nbsp; | <code>v128</code><br/>&nbsp;<br/>&nbsp; | <code>i64x2\.neg</code><br/><code>s64x2\.neg</code><br/><code>u64x2\.neg</code> | | <code><var>r</var><sub>i64x2</sub>[𝑛:0..1] ← -<var>p1</var><sub>i64x2</sub>[𝑛]</code> <br/>&nbsp;<br/>&nbsp; |
| <code>1</code> | <code>v128</code> | <code>f32x4\.neg</code> | | <code><var>r</var><sub>f32x4</sub>[𝑛:0..3] ← -<var>p1</var><sub>f32x4</sub>[𝑛]</code> |
| <code>1</code> | <code>v128</code> | <code>f64x2\.neg</code> | | <code><var>r</var><sub>f64x2</sub>[𝑛:0..1] ← -<var>p1</var><sub>f64x2</sub>[𝑛]</code> |


### 符号複写

| 引数 | 返値 | 命令 | OP | 備考 |
|:-:|:-:|:--|:--|:--|
| <code>2</code> | <code>f32</code> | <code>f32\.copysign</code> | | <code><var>r</var><sub>f32</sub> ← copsign(<var>p1</var><sub>f32</sub>, <var>p2</var><sub>f32</sub>)</code> |
| <code>2</code> | <code>f64</code> | <code>f64\.copysign</code> | | <code><var>r</var><sub>f64</sub> ← copsign(<var>p1</var><sub>f64</sub>, <var>p2</var><sub>f64</sub>)</code> |


### 加算

| 引数 | 返値 | 命令 | OP | 備考 |
|:-:|:-:|:--|:--|:--|
| <code>2</code><br/>&nbsp;<br/>&nbsp; | <code>i32</code><br/>&nbsp;<br/>&nbsp; | <code>i32\.add</code><br/><code>s32\.add</code><br/><code>u32\.add</code> | | <code><var>r</var><sub>i32</sub> ← <var>p1</var><sub>i32</sub> + <var>p2</var><sub>i32</sub></code> <br/>&nbsp;<br/>&nbsp; |
| <code>2</code><br/>&nbsp;<br/>&nbsp; | <code>i64</code><br/>&nbsp;<br/>&nbsp; | <code>i64\.add</code><br/><code>s64\.add</code><br/><code>u64\.add</code> | | <code><var>r</var><sub>i64</sub> ← <var>p1</var><sub>i64</sub> + <var>p2</var><sub>i64</sub></code> <br/>&nbsp;<br/>&nbsp; |
| <code>2</code> | <code>f32</code> | <code>f32\.add</code> | | <code><var>r</var><sub>f32</sub> ← <var>p1</var><sub>f32</sub> + <var>p2</var><sub>f32</sub></code> |
| <code>2</code> | <code>f64</code> | <code>f64\.add</code> | | <code><var>r</var><sub>f64</sub> ← <var>p1</var><sub>f64</sub> + <var>p2</var><sub>f64</sub></code> |
| | | | | |
| <code>2</code><br/>&nbsp;<br/>&nbsp; | <code>v128</code><br/>&nbsp;<br/>&nbsp; | <code>i8x16\.add</code><br/><code>s8x16\.add</code><br/><code>u8x16\.add</code> | | <code><var>r</var><sub>i8x16</sub>[𝑛:0..15] ← <var>p1</var><sub>i8x16</sub>[𝑛] + <var>p2</var><sub>i8x16</sub>[𝑛]</code> <br/>&nbsp;<br/>&nbsp; |
| <code>2</code><br/>&nbsp;<br/>&nbsp; | <code>v128</code><br/>&nbsp;<br/>&nbsp; | <code>i16x8\.add</code><br/><code>s16x8\.add</code><br/><code>u16x8\.add</code> | | <code><var>r</var><sub>i16x8</sub>[𝑛:0..7] ← <var>p1</var><sub>i16x8</sub>[𝑛] + <var>p2</var><sub>i16x8</sub>[𝑛]</code> <br/>&nbsp;<br/>&nbsp; |
| <code>2</code><br/>&nbsp;<br/>&nbsp; | <code>v128</code><br/>&nbsp;<br/>&nbsp; | <code>i32x4\.add</code><br/><code>s32x4\.add</code><br/><code>u32x4\.add</code> | | <code><var>r</var><sub>i32x4</sub>[𝑛:0..3] ← <var>p1</var><sub>i32x4</sub>[𝑛] + <var>p2</var><sub>i32x4</sub>[𝑛]</code> <br/>&nbsp;<br/>&nbsp; |
| <code>2</code><br/>&nbsp;<br/>&nbsp; | <code>v128</code><br/>&nbsp;<br/>&nbsp; | <code>i64x2\.add</code><br/><code>s64x2\.add</code><br/><code>u64x2\.add</code> | | <code><var>r</var><sub>i64x2</sub>[𝑛:0..1] ← <var>p1</var><sub>i64x2</sub>[𝑛] + <var>p2</var><sub>i64x2</sub>[𝑛]</code> <br/>&nbsp;<br/>&nbsp; |
| <code>2</code> | <code>v128</code> | <code>f32x4\.add</code> | | <code><var>r</var><sub>f32x4</sub>[𝑛:0..3] ← <var>p1</var><sub>f32x4</sub>[𝑛] + <var>p2</var><sub>f32x4</sub>[𝑛]</code> |
| <code>2</code> | <code>v128</code> | <code>f64x2\.add</code> | | <code><var>r</var><sub>f64x2</sub>[𝑛:0..1] ← <var>p1</var><sub>f64x2</sub>[𝑛] + <var>p2</var><sub>f64x2</sub>[𝑛]</code> |
| <code>2</code><br/>&nbsp; | <code>v128</code><br/>&nbsp; | <code>i8x16\.add\_sat\_s</code><br/><code>s8x16\.add\_sat</code> | | <code><var>r</var><sub>i8x16</sub>[𝑛:0..15] ← sat8s(<var>p1</var><sub>s8x16</sub>[𝑛] + <var>p2</var><sub>s8x16</sub>[𝑛])</code> <br/>&nbsp; |
| <code>2</code><br/>&nbsp; | <code>v128</code><br/>&nbsp; | <code>i8x16\.add\_sat\_u</code><br/><code>u8x16\.add\_sat</code> | | <code><var>r</var><sub>i8x16</sub>[𝑛:0..15] ← sat8u(<var>p1</var><sub>u8x16</sub>[𝑛] + <var>p2</var><sub>u8x16</sub>[𝑛])</code> <br/>&nbsp; |
| <code>2</code><br/>&nbsp; | <code>v128</code><br/>&nbsp; | <code>i16x8\.add\_sat\_s</code><br/><code>s16x8\.add\_sat</code> | | <code><var>r</var><sub>i16x8</sub>[𝑛:0..7] ← sat16s(<var>p1</var><sub>s16x8</sub>[𝑛] + <var>p2</var><sub>s16x8</sub>[𝑛])</code> <br/>&nbsp; |
| <code>2</code><br/>&nbsp; | <code>v128</code><br/>&nbsp; | <code>i16x8\.add\_sat\_u</code><br/><code>u16x8\.add\_sat</code> | | <code><var>r</var><sub>i16x8</sub>[𝑛:0..7] ← sat16u(<var>p1</var><sub>u16x8</sub>[𝑛] + <var>p2</var><sub>u16x8</sub>[𝑛])</code> <br/>&nbsp; |

| 引数 | 返値 | 命令 | OP | 備考 |
|:-:|:-:|:--|:--|:--|
| <code>2</code><br/>&nbsp;<br/>&nbsp;<br/>&nbsp; | <code>v128</code><br/>&nbsp;<br/>&nbsp;<br/>&nbsp; | <code>i16x8\.extadd\_pairwise\_i8x16\_s</code><br/><code>i16x8\.extadd\_pairwise\_s8x16</code><br/><code>s16x8\.extadd\_pairwise\_i8x16</code><br/><code>s16x8\.extadd\_pairwise\_s8x16</code> | | <code><var>r</var><sub>i16x8</sub>[𝑛:0..7] ← <var>p1</var><sub>s8x16</sub>[2𝑛] + <var>p1</var><sub>s8x16</sub>[2𝑛+1]</code> <br/>&nbsp;<br/>&nbsp;<br/>&nbsp; |
| <code>2</code><br/>&nbsp;<br/>&nbsp;<br/>&nbsp; | <code>v128</code><br/>&nbsp;<br/>&nbsp;<br/>&nbsp; | <code>i16x8\.extadd\_pairwise\_i8x16\_u</code><br/><code>i16x8\.extadd\_pairwise\_u8x16</code><br/><code>u16x8\.extadd\_pairwise\_i8x16</code><br/><code>u16x8\.extadd\_pairwise\_u8x16</code> | | <code><var>r</var><sub>i16x8</sub>[𝑛:0..7] ← <var>p1</var><sub>u8x16</sub>[2𝑛] + <var>p1</var><sub>u8x16</sub>[2𝑛+1]</code> <br/>&nbsp;<br/>&nbsp;<br/>&nbsp; |
| <code>2</code><br/>&nbsp;<br/>&nbsp;<br/>&nbsp; | <code>v128</code><br/>&nbsp;<br/>&nbsp;<br/>&nbsp; | <code>i32x4\.extadd\_pairwise\_i16x8\_s</code><br/><code>i32x4\.extadd\_pairwise\_s16x8</code><br/><code>s32x4\.extadd\_pairwise\_i16x8</code><br/><code>s32x4\.extadd\_pairwise\_s16x8</code> | | <code><var>r</var><sub>i32x4</sub>[𝑛:0..3] ← <var>p1</var><sub>s16x8</sub>[2𝑛] + <var>p1</var><sub>s16x8</sub>[2𝑛+1]</code> <br/>&nbsp;<br/>&nbsp;<br/>&nbsp; |
| <code>2</code><br/>&nbsp;<br/>&nbsp;<br/>&nbsp; | <code>v128</code><br/>&nbsp;<br/>&nbsp;<br/>&nbsp; | <code>i32x4\.extadd\_pairwise\_i16x8\_u</code><br/><code>i32x4\.extadd\_pairwise\_u16x8</code><br/><code>u32x4\.extadd\_pairwise\_i16x8</code><br/><code>u32x4\.extadd\_pairwise\_u16x8</code> | | <code><var>r</var><sub>i32x4</sub>[𝑛:0..3] ← <var>p1</var><sub>u16x8</sub>[2𝑛] + <var>p1</var><sub>u16x8</sub>[2𝑛+1]</code> <br/>&nbsp;<br/>&nbsp;<br/>&nbsp; |

### 減算

| 引数 | 返値 | 命令 | OP | 備考 |
|:-:|:-:|:--|:--|:--|
| <code>2</code><br/>&nbsp;<br/>&nbsp; | <code>i32</code><br/>&nbsp;<br/>&nbsp; | <code>i32\.sub</code><br/><code>s32\.sub</code><br/><code>u32\.sub</code> | | <code><var>r</var><sub>i32</sub> ← <var>p1</var><sub>i32</sub> - <var>p2</var><sub>i32</sub></code> <br/>&nbsp;<br/>&nbsp; |
| <code>2</code><br/>&nbsp;<br/>&nbsp; | <code>i64</code><br/>&nbsp;<br/>&nbsp; | <code>i64\.sub</code><br/><code>s64\.sub</code><br/><code>u64\.sub</code> | | <code><var>r</var><sub>i64</sub> ← <var>p1</var><sub>i64</sub> - <var>p2</var><sub>i64</sub></code> <br/>&nbsp;<br/>&nbsp; |
| <code>2</code> | <code>f32</code> | <code>f32\.sub</code> | | <code><var>r</var><sub>f32</sub> ← <var>p1</var><sub>f32</sub> - <var>p2</var><sub>f32</sub></code> |
| <code>2</code> | <code>f64</code> | <code>f64\.sub</code> | | <code><var>r</var><sub>f64</sub> ← <var>p1</var><sub>f64</sub> - <var>p2</var><sub>f64</sub></code> |
| | | | | |
| <code>2</code><br/>&nbsp;<br/>&nbsp; | <code>v128</code><br/>&nbsp;<br/>&nbsp; | <code>i8x16\.sub</code><br/><code>s8x16\.sub</code><br/><code>u8x16\.sub</code> | | <code><var>r</var><sub>i8x16</sub>[𝑛:0..15] ← <var>p1</var><sub>i8x16</sub>[𝑛] - <var>p2</var><sub>i8x16</sub>[𝑛]</code> <br/>&nbsp;<br/>&nbsp; |
| <code>2</code><br/>&nbsp;<br/>&nbsp; | <code>v128</code><br/>&nbsp;<br/>&nbsp; | <code>i16x8\.sub</code><br/><code>s16x8\.sub</code><br/><code>u16x8\.sub</code> | | <code><var>r</var><sub>i16x8</sub>[𝑛:0..7] ← <var>p1</var><sub>i16x8</sub>[𝑛] - <var>p2</var><sub>i16x8</sub>[𝑛]</code> <br/>&nbsp;<br/>&nbsp; |
| <code>2</code><br/>&nbsp;<br/>&nbsp; | <code>v128</code><br/>&nbsp;<br/>&nbsp; | <code>i32x4\.sub</code><br/><code>s32x4\.sub</code><br/><code>u32x4\.sub</code> | | <code><var>r</var><sub>i32x4</sub>[𝑛:0..3] ← <var>p1</var><sub>i32x4</sub>[𝑛] - <var>p2</var><sub>i32x4</sub>[𝑛]</code> <br/>&nbsp;<br/>&nbsp; |
| <code>2</code><br/>&nbsp;<br/>&nbsp; | <code>v128</code><br/>&nbsp;<br/>&nbsp; | <code>i64x2\.sub</code><br/><code>s64x2\.sub</code><br/><code>u64x2\.sub</code> | | <code><var>r</var><sub>i64x2</sub>[𝑛:0..1] ← <var>p1</var><sub>i64x2</sub>[𝑛] - <var>p2</var><sub>i64x2</sub>[𝑛]</code> <br/>&nbsp;<br/>&nbsp; |
| <code>2</code> | <code>v128</code> | <code>f32x4\.sub</code> | | <code><var>r</var><sub>f32x4</sub>[𝑛:0..3] ← <var>p1</var><sub>f32x4</sub>[𝑛] - <var>p2</var><sub>f32x4</sub>[𝑛]</code> |
| <code>2</code> | <code>v128</code> | <code>f64x2\.sub</code> | | <code><var>r</var><sub>f64x2</sub>[𝑛:0..1] ← <var>p1</var><sub>f64x2</sub>[𝑛] - <var>p2</var><sub>f64x2</sub>[𝑛]</code> |
| <code>2</code><br/>&nbsp; | <code>v128</code><br/>&nbsp; | <code>i8x16\.sub\_sat\_s</code><br/><code>s8x16\.sub\_sat</code> | | <code><var>r</var><sub>i8x16</sub>[𝑛:0..15] ← sat8s(<var>p1</var><sub>s8x16</sub>[𝑛] - <var>p2</var><sub>s8x16</sub>[𝑛])</code> <br/>&nbsp; |
| <code>2</code><br/>&nbsp; | <code>v128</code><br/>&nbsp; | <code>i8x16\.sub\_sat\_u</code><br/><code>u8x16\.sub\_sat</code> | | <code><var>r</var><sub>i8x16</sub>[𝑛:0..15] ← sat8u(<var>p1</var><sub>u8x16</sub>[𝑛] - <var>p2</var><sub>u8x16</sub>[𝑛])</code> <br/>&nbsp; |
| <code>2</code><br/>&nbsp; | <code>v128</code><br/>&nbsp; | <code>i16x8\.sub\_sat\_s</code><br/><code>s16x8\.sub\_sat</code> | | <code><var>r</var><sub>i16x8</sub>[𝑛:0..7] ← sat16s(<var>p1</var><sub>s16x8</sub>[𝑛] - <var>p2</var><sub>s16x8</sub>[𝑛])</code> <br/>&nbsp; |
| <code>2</code><br/>&nbsp; | <code>v128</code><br/>&nbsp; | <code>i16x8\.sub\_sat\_u</code><br/><code>u16x8\.sub\_sat</code> | | <code><var>r</var><sub>i16x8</sub>[𝑛:0..7] ← sat16u(<var>p1</var><sub>u16x8</sub>[𝑛] - <var>p2</var><sub>u16x8</sub>[𝑛])</code> <br/>&nbsp; |


### 乗算

| 引数 | 返値 | 命令 | OP | 備考 |
|:-:|:-:|:--|:--|:--|
| <code>2</code><br/>&nbsp;<br/>&nbsp; | <code>i32</code><br/>&nbsp;<br/>&nbsp; | <code>i32\.mul</code><br/><code>s32\.mul</code><br/><code>u32\.mul</code> | | <code><var>r</var><sub>i32</sub> ← <var>p1</var><sub>i32</sub> * <var>p2</var><sub>i32</sub></code> <br/>&nbsp;<br/>&nbsp; |
| <code>2</code><br/>&nbsp;<br/>&nbsp; | <code>i64</code><br/>&nbsp;<br/>&nbsp; | <code>i64\.mul</code><br/><code>s64\.mul</code><br/><code>u64\.mul</code> | | <code><var>r</var><sub>i64</sub> ← <var>p1</var><sub>i64</sub> * <var>p2</var><sub>i64</sub></code> <br/>&nbsp;<br/>&nbsp; |
| <code>2</code> | <code>f32</code> | <code>f32\.mul</code> | | <code><var>r</var><sub>f32</sub> ← <var>p1</var><sub>f32</sub> * <var>p2</var><sub>f32</sub></code> |
| <code>2</code> | <code>f64</code> | <code>f64\.mul</code> | | <code><var>r</var><sub>f64</sub> ← <var>p1</var><sub>f64</sub> * <var>p2</var><sub>f64</sub></code> |
| | | | | |
| <code>2</code><br/>&nbsp;<br/>&nbsp; | <code>v128</code><br/>&nbsp;<br/>&nbsp; | <code>i16x8\.mul</code><br/><code>s16x8\.mul</code><br/><code>u16x8\.mul</code> | | <code><var>r</var><sub>i16x8</sub>[𝑛:0..7] ← <var>p1</var><sub>i16x8</sub>[𝑛] \* <var>p2</var><sub>i16x8</sub>[𝑛]</code> <br/>&nbsp;<br/>&nbsp; |
| <code>2</code><br/>&nbsp;<br/>&nbsp; | <code>v128</code><br/>&nbsp;<br/>&nbsp; | <code>i32x4\.mul</code><br/><code>s32x4\.mul</code><br/><code>u32x4\.mul</code> | | <code><var>r</var><sub>i32x4</sub>[𝑛:0..3] ← <var>p1</var><sub>i32x4</sub>[𝑛] \* <var>p2</var><sub>i32x4</sub>[𝑛]</code> <br/>&nbsp;<br/>&nbsp; |
| <code>2</code><br/>&nbsp;<br/>&nbsp; | <code>v128</code><br/>&nbsp;<br/>&nbsp; | <code>i64x2\.mul</code><br/><code>s64x2\.mul</code><br/><code>u64x2\.mul</code> | | <code><var>r</var><sub>i64x2</sub>[𝑛:0..1] ← <var>p1</var><sub>i64x2</sub>[𝑛] \* <var>p2</var><sub>i64x2</sub>[𝑛]</code> <br/>&nbsp;<br/>&nbsp; |
| <code>2</code> | <code>v128</code> | <code>f32x4\.mul</code> | | <code><var>r</var><sub>f32x4</sub>[𝑛:0..3] ← <var>p1</var><sub>f32x4</sub>[𝑛] \* <var>p2</var><sub>f32x4</sub>[𝑛]</code> |
| <code>2</code> | <code>v128</code> | <code>f64x2\.mul</code> | | <code><var>r</var><sub>f64x2</sub>[𝑛:0..1] ← <var>p1</var><sub>f64x2</sub>[𝑛] \* <var>p2</var><sub>f64x2</sub>[𝑛]</code> |
| | | | | |
| <code>2</code> | <code>v128</code> | <code>i16x8\.extmul\_low\_i8x16\_s</code> | | <code><var>r</var><sub>i16x8</sub>[𝑛:0..7] ← <var>p1</var><sub>s8x16</sub>[2𝑛+0] * <var>p2</var><sub>s8x16</sub>[2𝑛+0]</code> |
| <code>2</code> | <code>v128</code> | <code>i16x8\.extmul\_low\_i8x16\_u</code> | | <code><var>r</var><sub>i16x8</sub>[𝑛:0..7] ← <var>p1</var><sub>u8x16</sub>[2𝑛+0] * <var>p2</var><sub>u8x16</sub>[2𝑛+0]</code> |
| <code>2</code> | <code>v128</code> | <code>i16x8\.extmul\_high\_i8x16\_s</code> | | <code><var>r</var><sub>i16x8</sub>[𝑛:0..7] ← <var>p1</var><sub>s8x16</sub>[2𝑛+1] * <var>p2</var><sub>s8x16</sub>[2𝑛+1]</code> |
| <code>2</code> | <code>v128</code> | <code>i16x8\.extmul\_high\_i8x16\_u</code> | | <code><var>r</var><sub>i16x8</sub>[𝑛:0..7] ← <var>p1</var><sub>u8x16</sub>[2𝑛+1] * <var>p2</var><sub>u8x16</sub>[2𝑛+1]</code> |
| <code>2</code> | <code>v128</code> | <code>i32x4\.extmul\_low\_i16x8\_s</code> | | <code><var>r</var><sub>i32x4</sub>[𝑛:0..3] ← <var>p1</var><sub>s16x8</sub>[2𝑛+0] * <var>p2</var><sub>s16x8</sub>[2𝑛+0]</code> |
| <code>2</code> | <code>v128</code> | <code>i32x4\.extmul\_low\_i16x8\_u</code> | | <code><var>r</var><sub>i32x4</sub>[𝑛:0..3] ← <var>p1</var><sub>u16x8</sub>[2𝑛+0] * <var>p2</var><sub>u16x8</sub>[2𝑛+0]</code> |
| <code>2</code> | <code>v128</code> | <code>i32x4\.extmul\_high\_i16x8\_s</code> | | <code><var>r</var><sub>i32x4</sub>[𝑛:0..3] ← <var>p1</var><sub>s16x8</sub>[2𝑛+1] * <var>p2</var><sub>s16x8</sub>[2𝑛+1]</code> |
| <code>2</code> | <code>v128</code> | <code>i32x4\.extmul\_high\_i16x8\_u</code> | | <code><var>r</var><sub>i32x4</sub>[𝑛:0..3] ← <var>p1</var><sub>u16x8</sub>[2𝑛+1] * <var>p2</var><sub>u16x8</sub>[2𝑛+1]</code> |
| <code>2</code> | <code>v128</code> | <code>i64x2\.extmul\_low\_i32x4\_s</code> | | <code><var>r</var><sub>i64x2</sub>[𝑛:0..1] ← <var>p1</var><sub>s32x4</sub>[2𝑛+0] * <var>p2</var><sub>s32x4</sub>[2𝑛+0]</code> |
| <code>2</code> | <code>v128</code> | <code>i64x2\.extmul\_low\_i32x4\_u</code> | | <code><var>r</var><sub>i64x2</sub>[𝑛:0..1] ← <var>p1</var><sub>u32x4</sub>[2𝑛+0] * <var>p2</var><sub>u32x4</sub>[2𝑛+0]</code> |
| <code>2</code> | <code>v128</code> | <code>i64x2\.extmul\_high\_i32x4\_s</code> | | <code><var>r</var><sub>i64x2</sub>[𝑛:0..1] ← <var>p1</var><sub>s32x4</sub>[2𝑛+1] * <var>p2</var><sub>s32x4</sub>[2𝑛+1]</code> |
| <code>2</code> | <code>v128</code> | <code>i64x2\.extmul\_high\_i32x4\_u</code> | | <code><var>r</var><sub>i64x2</sub>[𝑛:0..1] ← <var>p1</var><sub>u32x4</sub>[2𝑛+1] * <var>p2</var><sub>u32x4</sub>[2𝑛+1]</code> |
| | | | | |
| <code>2</code><br/>&nbsp; | <code>v128</code><br/>&nbsp; | <code>i16x8\.q15mulr\_sat\_s</code><br/><code>s16x8\.q15mulr\_sat</code> | | <code><var>r</var><sub>i16x8</sub>[𝑛:0..7] ← sat16s(t)</code><br/><code>    t = i15mul(<var>p1</var><sub>s16x8</sub>[𝑛], <var>p2</var><sub>s16x8</sub>[𝑛])</code> |


### 除算

| 引数 | 返値 | 命令 | OP | 備考 |
|:-:|:-:|:--|:--|:--|
| <code>2</code><br/>&nbsp; | <code>i32</code><br/>&nbsp; | <code>i32\.div\_s</code><br/><code>s32\.div</code> | | <code><var>r</var><sub>s32</sub> ← <var>p1</var><sub>s32</sub> / <var>p2</var><sub>s32</sub></code> <br/>&nbsp; |
| <code>2</code><br/>&nbsp; | <code>i32</code><br/>&nbsp; | <code>i32\.div\_u</code><br/><code>u32\.div</code> | | <code><var>r</var><sub>u32</sub> ← <var>p1</var><sub>u32</sub> / <var>p2</var><sub>u32</sub></code> <br/>&nbsp; |
| <code>2</code><br/>&nbsp; | <code>i64</code><br/>&nbsp; | <code>i64\.div\_s</code><br/><code>s64\.div</code> | | <code><var>r</var><sub>s64</sub> ← <var>p1</var><sub>s64</sub> / <var>p2</var><sub>s64</sub></code> <br/>&nbsp; |
| <code>2</code><br/>&nbsp; | <code>i64</code><br/>&nbsp; | <code>i64\.div\_u</code><br/><code>u64\.div</code> | | <code><var>r</var><sub>u64</sub> ← <var>p1</var><sub>u64</sub> / <var>p2</var><sub>u64</sub></code> <br/>&nbsp; |
| <code>2</code> | <code>f32</code> | <code>f32\.div</code> | | <code><var>r</var><sub>f32</sub> ← <var>p1</var><sub>f32</sub> / <var>p2</var><sub>f32</sub></code> |
| <code>2</code> | <code>f64</code> | <code>f64\.div</code> | | <code><var>r</var><sub>f64</sub> ← <var>p1</var><sub>f64</sub> / <var>p2</var><sub>f64</sub></code> |
| | | | | |
| <code>2</code> | <code>v128</code> | <code>f32x4\.div</code> | | <code><var>r</var><sub>f32x4</sub>[𝑛:0..3] ← <var>p1</var><sub>f32x4</sub>[𝑛] / <var>p2</var><sub>f32x4</sub>[𝑛]</code> |
| <code>2</code> | <code>v128</code> | <code>f64x2\.div</code> | | <code><var>r</var><sub>f64x2</sub>[𝑛:0..1] ← <var>p1</var><sub>f64x2</sub>[𝑛] / <var>p2</var><sub>f64x2</sub>[𝑛]</code> |


### 剰余

| 引数 | 返値 | 命令 | OP | 備考 |
|:-:|:-:|:--|:--|:--|
| <code>2</code><br/>&nbsp; | <code>i32</code><br/>&nbsp; | <code>i32\.rem\_s</code><br/><code>s32\.rem</code> | | <code><var>r</var><sub>i32</sub> ← <var>p1</var><sub>s32</sub> % <var>p2</var><sub>s32</sub></code> <br/>&nbsp; |
| <code>2</code><br/>&nbsp; | <code>i32</code><br/>&nbsp; | <code>i32\.rem\_u</code><br/><code>u32\.rem</code> | | <code><var>r</var><sub>i32</sub> ← <var>p1</var><sub>u32</sub> % <var>p2</var><sub>u32</sub></code> <br/>&nbsp; |
| <code>2</code><br/>&nbsp; | <code>i64</code><br/>&nbsp; | <code>i64\.rem\_s</code><br/><code>s64\.rem</code> | | <code><var>r</var><sub>i64</sub> ← <var>p1</var><sub>s64</sub> % <var>p2</var><sub>s64</sub></code> <br/>&nbsp; |
| <code>2</code><br/>&nbsp; | <code>i64</code><br/>&nbsp; | <code>i64\.rem\_u</code><br/><code>u64\.rem</code> | | <code><var>r</var><sub>i64</sub> ← <var>p1</var><sub>u64</sub> % <var>p2</var><sub>u64</sub></code> <br/>&nbsp; |


### 加算平均

| 引数 | 返値 | 命令 | OP | 備考 |
|:-:|:-:|:--|:--|:--|
| <code>2</code><br/>&nbsp; | <code>v128</code><br/>&nbsp; | <code>i8x16\.avgr\_u</code><br/><code>u8x16\.avgr</code> | | <code><var>r</var><sub>i8x16</sub>[𝑛:0..15] ← avgr(<var>p1</var><sub>u8x16</sub>[𝑛], <var>p2</var><sub>u8x16</sub>[𝑛])</code> <br/>&nbsp; |
| <code>2</code><br/>&nbsp; | <code>v128</code><br/>&nbsp; | <code>i16x8\.avgr\_u</code><br/><code>u16x8\.avgr</code> | | <code><var>r</var><sub>i16x8</sub>[𝑛:0..7] ← avgr(<var>p1</var><sub>u16x8</sub>[𝑛], <var>p2</var><sub>u16x8</sub>[𝑛])</code> <br/>&nbsp; |


### 平方根

| 引数 | 返値 | 命令 | OP | 備考 |
|:-:|:-:|:--|:--|:--|
| <code>1</code> | <code>f32</code> | <code>f32\.sqrt</code> | | <code><var>r</var><sub>f32</sub> ← sqrt(<var>p1</var><sub>f32</sub>)</code> |
| <code>1</code> | <code>f64</code> | <code>f64\.sqrt</code> | | <code><var>r</var><sub>f64</sub> ← sqrt(<var>p1</var><sub>f64</sub>)</code> |
| | | | | |
| <code>1</code> | <code>v128</code> | <code>f32x4\.sqrt</code> | | <code><var>r</var><sub>f32x4</sub>[𝑛:0..3] ← sqrt(<var>p1</var><sub>f32x4</sub>[𝑛])</code> |
| <code>1</code> | <code>v128</code> | <code>f64x2\.sqrt</code> | | <code><var>r</var><sub>f64x2</sub>[𝑛:0..1] ← sqrt(<var>p1</var><sub>f64x2</sub>[𝑛])</code> |


### 内積

| 引数 | 返値 | 命令 | OP | 備考 |
|:-:|:-:|:--|:--|:--|
| <code>2</code><br/>&nbsp;<br/>&nbsp;<br/>&nbsp; | <code>v128</code><br/>&nbsp;<br/>&nbsp;<br/>&nbsp; | <code>i32x4\.dot\_i16x8\_s</code><br/><code>i32x4\.dot\_s16x8</code><br/><code>s32x4\.dot\_i16x8</code><br/><code>s32x4\.dot\_s16x8</code> | | <code><var>r</var><sub>i32x4</sub>[𝑛:0..3] ← x + y</code><br/><code>    x = <var>p1</var><sub>s16x8</sub>[2𝑛+0] * <var>p2</var><sub>s16x8</sub>[2𝑛+0]</code><br/><code>    y = <var>p1</var><sub>s16x8</sub>[2𝑛+1] * <var>p2</var><sub>s16x8</sub>[2𝑛+1]</code> <br/>&nbsp; |


## シフト・ローテート


### 左シフト

| 引数 | 返値 | 命令 | OP | 備考 |
|:-:|:-:|:--|:--|:--|
| <code>2</code><br/>&nbsp;<br/>&nbsp; | <code>i32</code><br/>&nbsp;<br/>&nbsp; | <code>i32\.shl</code><br/><code>s32\.shl</code><br/><code>u32\.shl</code> | | <code><var>r</var><sub>i32</sub> ← <var>p1</var><sub>i32</sub> &lt;&lt; (<var>p2</var><sub>i32</sub> &amp; 31)</code> <br/>&nbsp;<br/>&nbsp; |
| <code>2</code><br/>&nbsp;<br/>&nbsp; | <code>i64</code><br/>&nbsp;<br/>&nbsp; | <code>i64\.shl</code><br/><code>s64\.shl</code><br/><code>u64\.shl</code> | | <code><var>r</var><sub>i64</sub> ← <var>p1</var><sub>i64</sub> &lt;&lt; (<var>p2</var><sub>i64</sub> &amp; 63)</code> <br/>&nbsp;<br/>&nbsp; |
| | | | | |
| <code>2</code><br/>&nbsp;<br/>&nbsp; | <code>v128</code><br/>&nbsp;<br/>&nbsp; | <code>i8x16\.shl</code><br/><code>s8x16\.shl</code><br/><code>u8x16\.shl</code> | | <code><var>r</var><sub>i8x16</sub>[𝑛:0..15] ← <var>p1</var><sub>i8x16</sub>[𝑛] &lt;&lt; (<var>p2</var><sub>i32</sub> &amp; 7)</code> <br/>&nbsp;<br/>&nbsp; |
| <code>2</code><br/>&nbsp;<br/>&nbsp; | <code>v128</code><br/>&nbsp;<br/>&nbsp; | <code>i16x8\.shl</code><br/><code>s16x8\.shl</code><br/><code>u16x8\.shl</code> | | <code><var>r</var><sub>i16x8</sub>[𝑛:0..7] ← <var>p1</var><sub>i16x8</sub>[𝑛] &lt;&lt; (<var>p2</var><sub>i32</sub> &amp; 15)</code> <br/>&nbsp;<br/>&nbsp; |
| <code>2</code><br/>&nbsp;<br/>&nbsp; | <code>v128</code><br/>&nbsp;<br/>&nbsp; | <code>i32x4\.shl</code><br/><code>s32x4\.shl</code><br/><code>u32x4\.shl</code> | | <code><var>r</var><sub>i32x4</sub>[𝑛:0..3] ← <var>p1</var><sub>i32x4</sub>[𝑛] &lt;&lt; (<var>p2</var><sub>i32</sub> &amp; 31)</code> <br/>&nbsp;<br/>&nbsp; |
| <code>2</code><br/>&nbsp;<br/>&nbsp; | <code>v128</code><br/>&nbsp;<br/>&nbsp; | <code>i64x2\.shl</code><br/><code>s64x2\.shl</code><br/><code>u64x2\.shl</code> | | <code><var>r</var><sub>i64x2</sub>[𝑛:0..1] ← <var>p1</var><sub>i64x2</sub>[𝑛] &lt;&lt; (<var>p2</var><sub>i32</sub> &amp; 63)</code> <br/>&nbsp;<br/>&nbsp; |


### 符号あり右シフト

| 引数 | 返値 | 命令 | OP | 備考 |
|:-:|:-:|:--|:--|:--|
| <code>2</code><br/>&nbsp; | <code>i32</code><br/>&nbsp; | <code>i32\.shr\_s</code><br/><code>s32\.shr</code> | | <code><var>r</var><sub>i32</sub> ← <var>p1</var><sub>s32</sub> &gt;&gt; (<var>p2</var><sub>i32</sub> &amp; 31)</code> <br/>&nbsp; |
| <code>2</code><br/>&nbsp; | <code>i64</code><br/>&nbsp; | <code>i64\.shr\_s</code><br/><code>s64\.shr</code> | | <code><var>r</var><sub>i64</sub> ← <var>p1</var><sub>s64</sub> &gt;&gt; (<var>p2</var><sub>i64</sub> &amp; 63)</code> <br/>&nbsp; |
| | | | | |
| <code>2</code><br/>&nbsp; | <code>v128</code><br/>&nbsp; | <code>i8x16\.shr\_s</code><br/><code>s8x16\.shr</code> | | <code><var>r</var><sub>i8x16</sub>[𝑛:0..15] ← <var>p1</var><sub>s8x16</sub>[𝑛] &gt;&gt; (<var>p2</var><sub>i32</sub> &amp; 7)</code> <br/>&nbsp; |
| <code>2</code><br/>&nbsp; | <code>v128</code><br/>&nbsp; | <code>i16x8\.shr\_s</code><br/><code>s16x8\.shr</code> | | <code><var>r</var><sub>i16x8</sub>[𝑛:0..7] ← <var>p1</var><sub>s16x8</sub>[𝑛] &gt;&gt; (<var>p2</var><sub>i32</sub> &amp; 15)</code> <br/>&nbsp; |
| <code>2</code><br/>&nbsp; | <code>v128</code><br/>&nbsp; | <code>i32x4\.shr\_s</code><br/><code>s32x4\.shr</code> | | <code><var>r</var><sub>i32x4</sub>[𝑛:0..3] ← <var>p1</var><sub>s32x4</sub>[𝑛] &gt;&gt; (<var>p2</var><sub>i32</sub> &amp; 31)</code> <br/>&nbsp; |
| <code>2</code><br/>&nbsp; | <code>v128</code><br/>&nbsp; | <code>i64x2\.shr\_s</code><br/><code>s64x2\.shr</code> | | <code><var>r</var><sub>i64x2</sub>[𝑛:0..1] ← <var>p1</var><sub>s64x2</sub>[𝑛] &gt;&gt; (<var>p2</var><sub>i32</sub> &amp; 63)</code> <br/>&nbsp; |


### 符号なし右シフト

| 引数 | 返値 | 命令 | OP | 備考 |
|:-:|:-:|:--|:--|:--|
| <code>2</code><br/>&nbsp; | <code>i32</code><br/>&nbsp; | <code>i32\.shr\_u</code><br/><code>u32\.shr</code> | | <code><var>r</var><sub>i32</sub> ← <var>p1</var><sub>u32</sub> &gt;&gt;&gt; (<var>p2</var><sub>i32</sub> &amp; 31)</code> <br/>&nbsp; |
| <code>2</code><br/>&nbsp; | <code>i64</code><br/>&nbsp; | <code>i64\.shr\_u</code><br/><code>u64\.shr</code> | | <code><var>r</var><sub>i64</sub> ← <var>p1</var><sub>u64</sub> &gt;&gt;&gt; (<var>p2</var><sub>i64</sub> &amp; 63)</code> <br/>&nbsp; |
| | | | | |
| <code>2</code><br/>&nbsp; | <code>v128</code><br/>&nbsp; | <code>i8x16\.shr\_u</code><br/><code>u8x16\.shr</code> | | <code><var>r</var><sub>i8x16</sub>[𝑛:0..15] ← <var>p1</var><sub>u8x16</sub>[𝑛] &gt;&gt;&gt; (<var>p2</var><sub>i32</sub> &amp; 7)</code> <br/>&nbsp; |
| <code>2</code><br/>&nbsp; | <code>v128</code><br/>&nbsp; | <code>i16x8\.shr\_u</code><br/><code>u16x8\.shr</code> | | <code><var>r</var><sub>i16x8</sub>[𝑛:0..7] ← <var>p1</var><sub>u16x8</sub>[𝑛] &gt;&gt;&gt; (<var>p2</var><sub>i32</sub> &amp; 15)</code> <br/>&nbsp; |
| <code>2</code><br/>&nbsp; | <code>v128</code><br/>&nbsp; | <code>i32x4\.shr\_u</code><br/><code>u32x4\.shr</code> | | <code><var>r</var><sub>i32x4</sub>[𝑛:0..3] ← <var>p1</var><sub>u32x4</sub>[𝑛] &gt;&gt;&gt; (<var>p2</var><sub>i32</sub> &amp; 31)</code> <br/>&nbsp; |
| <code>2</code><br/>&nbsp; | <code>v128</code><br/>&nbsp; | <code>i64x2\.shr\_u</code><br/><code>u64x2\.shr</code> | | <code><var>r</var><sub>i64x2</sub>[𝑛:0..1] ← <var>p1</var><sub>u64x2</sub>[𝑛] &gt;&gt;&gt; (<var>p2</var><sub>i32</sub> &amp; 63)</code> <br/>&nbsp; |


### ローテート

| 引数 | 返値 | 命令 | OP | 備考 |
|:-:|:-:|:--|:--|:--|
| <code>2</code><br/>&nbsp;<br/>&nbsp; | <code>i32</code><br/>&nbsp;<br/>&nbsp; | <code>i32\.rotl</code><br/><code>s32\.rotl</code><br/><code>u32\.rotl</code> | | <code><var>r</var><sub>i32</sub> ← rotl32(<var>p1</var><sub>i32</sub>)</code> <br/>&nbsp;<br/>&nbsp; |
| <code>2</code><br/>&nbsp;<br/>&nbsp; | <code>i64</code><br/>&nbsp;<br/>&nbsp; | <code>i64\.rotl</code><br/><code>s64\.rotl</code><br/><code>u64\.rotl</code> | | <code><var>r</var><sub>i64</sub> ← rotl64(<var>p1</var><sub>i64</sub>)</code> <br/>&nbsp;<br/>&nbsp; |
| <code>2</code><br/>&nbsp;<br/>&nbsp; | <code>i32</code><br/>&nbsp;<br/>&nbsp; | <code>i32\.rotr</code><br/><code>s32\.rotr</code><br/><code>u32\.rotr</code> | | <code><var>r</var><sub>i32</sub> ← rotr32(<var>p1</var><sub>i32</sub>)</code> <br/>&nbsp;<br/>&nbsp; |
| <code>2</code><br/>&nbsp;<br/>&nbsp; | <code>i64</code><br/>&nbsp;<br/>&nbsp; | <code>i64\.rotr</code><br/><code>s64\.rotr</code><br/><code>u64\.rotr</code> | | <code><var>r</var><sub>i64</sub> ← rotr64(<var>p1</var><sub>i64</sub>)</code> <br/>&nbsp;<br/>&nbsp; |


## 選択


### 小さい方

| 引数 | 返値 | 命令 | OP | 備考 |
|:-:|:-:|:--|:--|:--|
| <code>2</code> | <code>f32</code> | <code>f32\.min</code> | | <code><var>r</var><sub>f32</sub> ← min(<var>p1</var><sub>f32</sub>, <var>p2</var><sub>f32</sub>)</code> |
| <code>2</code> | <code>f64</code> | <code>f64\.min</code> | | <code><var>r</var><sub>f64</sub> ← min(<var>p1</var><sub>f64</sub>, <var>p2</var><sub>f64</sub>)</code> |
| | | | | |
| <code>2</code><br/>&nbsp; | <code>v128</code><br/>&nbsp; | <code>i8x16\.min\_s</code><br/><code>s8x16\.min</code> | | <code><var>r</var><sub>i8x16</sub>[𝑛:0..15] ← min(<var>p1</var><sub>s8x16</sub>[𝑛], <var>p2</var><sub>s8x16</sub>[𝑛])</code> <br/>&nbsp; |
| <code>2</code><br/>&nbsp; | <code>v128</code><br/>&nbsp; | <code>i8x16\.min\_u</code><br/><code>u8x16\.min</code> | | <code><var>r</var><sub>i8x16</sub>[𝑛:0..15] ← min(<var>p1</var><sub>u8x16</sub>[𝑛], <var>p2</var><sub>u8x16</sub>[𝑛])</code> <br/>&nbsp; |
| <code>2</code><br/>&nbsp; | <code>v128</code><br/>&nbsp; | <code>i16x8\.min\_s</code><br/><code>s16x8\.min</code> | | <code><var>r</var><sub>i16x8</sub>[𝑛:0..7] ← min(<var>p1</var><sub>s16x8</sub>[𝑛], <var>p2</var><sub>s16x8</sub>[𝑛])</code> <br/>&nbsp; |
| <code>2</code><br/>&nbsp; | <code>v128</code><br/>&nbsp; | <code>i16x8\.min\_u</code><br/><code>u16x8\.min</code> | | <code><var>r</var><sub>i16x8</sub>[𝑛:0..7] ← min(<var>p1</var><sub>u16x8</sub>[𝑛], <var>p2</var><sub>u16x8</sub>[𝑛])</code> <br/>&nbsp; |
| <code>2</code><br/>&nbsp; | <code>v128</code><br/>&nbsp; | <code>i32x4\.min\_s</code><br/><code>s32x4\.min</code> | | <code><var>r</var><sub>i32x4</sub>[𝑛:0..3] ← min(<var>p1</var><sub>s32x4</sub>[𝑛], <var>p2</var><sub>s32x4</sub>[𝑛])</code> <br/>&nbsp; |
| <code>2</code><br/>&nbsp; | <code>v128</code><br/>&nbsp; | <code>i32x4\.min\_u</code><br/><code>u32x4\.min</code> | | <code><var>r</var><sub>i32x4</sub>[𝑛:0..3] ← min(<var>p1</var><sub>u32x4</sub>[𝑛], <var>p2</var><sub>u32x4</sub>[𝑛])</code> <br/>&nbsp; |
| <code>2</code> | <code>v128</code> | <code>f32x4\.min</code> | | <code><var>r</var><sub>f32x4</sub>[𝑛:0..3] ← min(<var>p1</var><sub>f32x4</sub>[𝑛], <var>p2</var><sub>f32x4</sub>[𝑛])</code> |
| <code>2</code> | <code>v128</code> | <code>f64x2\.min</code> | | <code><var>r</var><sub>f64x2</sub>[𝑛:0..1] ← min(<var>p1</var><sub>f64x2</sub>[𝑛], <var>p2</var><sub>f64x2</sub>[𝑛])</code> |
| <code>2</code> | <code>v128</code> | <code>f32x4\.pmin</code> | | <code><var>r</var><sub>f32x4</sub>[𝑛:0..3] ← pmin(<var>p1</var><sub>f32x4</sub>[𝑛], <var>p2</var><sub>f32x4</sub>[𝑛])</code> |
| <code>2</code> | <code>v128</code> | <code>f64x2\.pmin</code> | | <code><var>r</var><sub>f64x2</sub>[𝑛:0..1] ← pmin(<var>p1</var><sub>f64x2</sub>[𝑛], <var>p2</var><sub>f64x2</sub>[𝑛])</code> |


### 大きい方

| 引数 | 返値 | 命令 | OP | 備考 |
|:-:|:-:|:--|:--|:--|
| <code>2</code> | <code>f32</code> | <code>f32\.max</code> | | <code><var>r</var><sub>f32</sub> ← max(<var>p1</var><sub>f32</sub>, <var>p2</var><sub>f32</sub>)</code> |
| <code>2</code> | <code>f64</code> | <code>f64\.max</code> | | <code><var>r</var><sub>f64</sub> ← max(<var>p1</var><sub>f64</sub>, <var>p2</var><sub>f64</sub>)</code> |
| | | | | |
| <code>2</code><br/>&nbsp; | <code>v128</code><br/>&nbsp; | <code>i8x16\.max\_s</code><br/><code>s8x16\.max</code> | | <code><var>r</var><sub>i8x16</sub>[𝑛:0..15] ← max(<var>p1</var><sub>s8x16</sub>[𝑛], <var>p2</var><sub>s8x16</sub>[𝑛])</code> <br/>&nbsp; |
| <code>2</code><br/>&nbsp; | <code>v128</code><br/>&nbsp; | <code>i8x16\.max\_u</code><br/><code>u8x16\.max</code> | | <code><var>r</var><sub>i8x16</sub>[𝑛:0..15] ← max(<var>p1</var><sub>u8x16</sub>[𝑛], <var>p2</var><sub>u8x16</sub>[𝑛])</code> <br/>&nbsp; |
| <code>2</code><br/>&nbsp; | <code>v128</code><br/>&nbsp; | <code>i16x8\.max\_s</code><br/><code>s16x8\.max</code> | | <code><var>r</var><sub>i16x8</sub>[𝑛:0..7] ← max(<var>p1</var><sub>s16x8</sub>[𝑛], <var>p2</var><sub>s16x8</sub>[𝑛])</code> <br/>&nbsp; |
| <code>2</code><br/>&nbsp; | <code>v128</code><br/>&nbsp; | <code>i16x8\.max\_u</code><br/><code>u16x8\.max</code> | | <code><var>r</var><sub>i16x8</sub>[𝑛:0..7] ← max(<var>p1</var><sub>u16x8</sub>[𝑛], <var>p2</var><sub>u16x8</sub>[𝑛])</code> <br/>&nbsp; |
| <code>2</code><br/>&nbsp; | <code>v128</code><br/>&nbsp; | <code>i32x4\.max\_s</code><br/><code>s32x4\.max</code> | | <code><var>r</var><sub>i32x4</sub>[𝑛:0..3] ← max(<var>p1</var><sub>s32x4</sub>[𝑛], <var>p2</var><sub>s32x4</sub>[𝑛])</code> <br/>&nbsp; |
| <code>2</code><br/>&nbsp; | <code>v128</code><br/>&nbsp; | <code>i32x4\.max\_u</code><br/><code>u32x4\.max</code> | | <code><var>r</var><sub>i32x4</sub>[𝑛:0..3] ← max(<var>p1</var><sub>u32x4</sub>[𝑛], <var>p2</var><sub>u32x4</sub>[𝑛])</code> <br/>&nbsp; |
| <code>2</code> | <code>v128</code> | <code>f32x4\.max</code> | | <code><var>r</var><sub>f32x4</sub>[𝑛:0..3] ← max(<var>p1</var><sub>f32x4</sub>[𝑛], <var>p2</var><sub>f32x4</sub>[𝑛])</code> |
| <code>2</code> | <code>v128</code> | <code>f64x2\.max</code> | | <code><var>r</var><sub>f64x2</sub>[𝑛:0..1] ← max(<var>p1</var><sub>f64x2</sub>[𝑛], <var>p2</var><sub>f64x2</sub>[𝑛])</code> |
| <code>2</code> | <code>v128</code> | <code>f32x4\.pmax</code> | | <code><var>r</var><sub>f32x4</sub>[𝑛:0..3] ← pmax(<var>p1</var><sub>f32x4</sub>[𝑛], <var>p2</var><sub>f32x4</sub>[𝑛])</code> |
| <code>2</code> | <code>v128</code> | <code>f64x2\.pmax</code> | | <code><var>r</var><sub>f64x2</sub>[𝑛:0..1] ← pmax(<var>p1</var><sub>f64x2</sub>[𝑛], <var>p2</var><sub>f64x2</sub>[𝑛])</code> |


### 条件選択

| 引数 | 返値 | 命令 | OP | 備考 |
|:-:|:-:|:--|:--|:--|
| <code>3</code> | <code>any</code> | <code>select</code> | | <code><var>r</var> ← <var>p3</var><sub>i32</sub> ? <var>p1</var> : <var>p2</var></code> |
| <code>3</code> | <code>any</code> | <code>select</code> | <code>valtype,valtype,...</code> | 同上 (<code>valtype</code>リスト形式は将来予約) |


## ビット計数

| 引数 | 返値 | 命令 | OP | 備考 |
|:-:|:-:|:--|:--|:--|
| <code>1</code><br/>&nbsp;<br/>&nbsp; | <code>i32</code><br/>&nbsp;<br/>&nbsp; | <code>i32\.clz</code><br/><code>s32\.clz</code><br/><code>u32\.clz</code> | | <code><var>r</var><sub>i32</sub> ← clz32(<var>p1</var><sub>i32</sub>)</code> <br/>&nbsp;<br/>&nbsp; |
| <code>1</code><br/>&nbsp;<br/>&nbsp; | <code>i64</code><br/>&nbsp;<br/>&nbsp; | <code>i64\.clz</code><br/><code>s64\.clz</code><br/><code>u64\.clz</code> | | <code><var>r</var><sub>i64</sub> ← clz64(<var>p1</var><sub>i64</sub>)</code> <br/>&nbsp;<br/>&nbsp; |
| <code>1</code><br/>&nbsp;<br/>&nbsp; | <code>i32</code><br/>&nbsp;<br/>&nbsp; | <code>i32\.ctz</code><br/><code>s32\.ctz</code><br/><code>u32\.ctz</code> | | <code><var>r</var><sub>i32</sub> ← ctz32(<var>p1</var><sub>i32</sub>)</code> <br/>&nbsp;<br/>&nbsp; |
| <code>1</code><br/>&nbsp;<br/>&nbsp; | <code>i64</code><br/>&nbsp;<br/>&nbsp; | <code>i64\.ctz</code><br/><code>s64\.ctz</code><br/><code>u64\.ctz</code> | | <code><var>r</var><sub>i64</sub> ← ctz64(<var>p1</var><sub>i64</sub>)</code> <br/>&nbsp;<br/>&nbsp; |
| <code>1</code><br/>&nbsp;<br/>&nbsp; | <code>i32</code><br/>&nbsp;<br/>&nbsp; | <code>i32\.popcnt</code><br/><code>s32\.popcnt</code><br/><code>u32\.popcnt</code> | | <code><var>r</var><sub>i32</sub> ← popcnt(<var>p1</var><sub>i32</sub>)</code> <br/>&nbsp;<br/>&nbsp; |
| <code>1</code><br/>&nbsp;<br/>&nbsp; | <code>i64</code><br/>&nbsp;<br/>&nbsp; | <code>i64\.popcnt</code><br/><code>s64\.popcnt</code><br/><code>u64\.popcnt</code> | | <code><var>r</var><sub>i64</sub> ← popcnt(<var>p1</var><sub>i64</sub>)</code> <br/>&nbsp;<br/>&nbsp; |
| | | | | |
| <code>1</code><br/>&nbsp;<br/>&nbsp; | <code>v128</code><br/>&nbsp;<br/>&nbsp; | <code>i8x16\.popcnt</code><br/><code>s8x16\.popcnt</code><br/><code>u8x16\.popcnt</code> | | <code><var>r</var><sub>i8x16</sub>[𝑛:0..15] ← popcnt(<var>p1</var><sub>i8x16</sub>[𝑛])</code> <br/>&nbsp;<br/>&nbsp; |


## 符号の集積

| 引数 | 返値 | 命令 | OP | 備考 |
|:-:|:-:|:--|:--|:--|
| <code>1</code><br/>&nbsp;<br/>&nbsp; | <code>i32</code><br/>&nbsp;<br/>&nbsp; | <code>i8x16\.bitmask</code><br/><code>s8x16\.bitmask</code><br/><code>u8x16\.bitmask</code> | | <code><var>r</var><sub>i32</sub> ← 0 </code><br><code><var>r</var><sub>i32</sub> &vert;= sign8(<var>p1</var><sub>i8x16</sub>[𝑛:0..15]) &lt;&lt; 𝑛</code> <br/>&nbsp; |
| <code>1</code><br/>&nbsp;<br/>&nbsp; | <code>i32</code><br/>&nbsp;<br/>&nbsp; | <code>i16x8\.bitmask</code><br/><code>s16x8\.bitmask</code><br/><code>u16x8\.bitmask</code> | | <code><var>r</var><sub>i32</sub> ← 0 </code><br><code><var>r</var><sub>i32</sub> &vert;= sign16(<var>p1</var><sub>i16x8</sub>[𝑛:0..7]) &lt;&lt; 𝑛</code> <br/>&nbsp; |
| <code>1</code><br/>&nbsp;<br/>&nbsp; | <code>i32</code><br/>&nbsp;<br/>&nbsp; | <code>i32x4\.bitmask</code><br/><code>s32x4\.bitmask</code><br/><code>u32x4\.bitmask</code> | | <code><var>r</var><sub>i32</sub> ← 0 </code><br><code><var>r</var><sub>i32</sub> &vert;= sign32(<var>p1</var><sub>i32x4</sub>[𝑛:0..3]) &lt;&lt; 𝑛</code> <br/>&nbsp; |
| <code>1</code><br/>&nbsp;<br/>&nbsp; | <code>i32</code><br/>&nbsp;<br/>&nbsp; | <code>i64x2\.bitmask</code><br/><code>s64x2\.bitmask</code><br/><code>u64x2\.bitmask</code> | | <code><var>r</var><sub>i32</sub> ← 0 </code><br><code><var>r</var><sub>i32</sub> &vert;= sign64(<var>p1</var><sub>i64x2</sub>[𝑛:0..1]) &lt;&lt; 𝑛</code> <br/>&nbsp; |


## スタック操作

| 引数 | 返値 | 命令 | OP | 備考 |
|:-:|:-:|:--|:--|:--|
| <code>1</code> | | <code>drop</code> | | <code><var>p1</var></code> を破棄 |


## 関数ポインタ

| 引数 | 返値 | 命令 | OP | 備考 (<code>𝑥 = funcidx</code>) |
|:-:|:-:|:--|:--|:--|
| <code>1</code> | <code>i32</code> | <code>ref\.is\_null</code> | | <code><var>r</var><sub>i32</sub> ← (<var>p1</var><sub>func</sub> == null)</code> |
| <code>0</code> | <code>func</code> | <code>ref\.null</code> | <code>reftype</code> | <code><var>r</var><sub><var>func</var></sub> ← null</code> |
| <code>0</code> | <code>func</code> | <code>ref\.func</code> | <code>funcidx</code> | <code><var>r</var><sub>func</sub> ← FUNC[𝑥]</code> |


## テーブル領域操作

| 引数 | 返値 | 命令 | OP<code>𝑥,𝑦</code> | 備考 |
|:-:|:-:|:--|:--|:--|
| <code>0</code> | <code>i32</code> | <code>table\.size</code> | <code>tableidx</code> |  <code><var>r</var><sub>i32</sub> ← (TAB[𝑥] のサイズ)</code> |
| <code>0</code> | <code>i32</code> | <code>table\.grow</code> | <code>tableidx</code> | <code><var>r</var><sub>i32</sub> ← (TAB[𝑥] のサイズ)</code> <br> <code>TAB[𝑥]</code> をサイズ <code><var>p2</var><sub>i32</sub></code> に変更(内容は <code><var>p1</var></code>) |
| <code>3</code> | | <code>table\.fill</code> | <code>tableidx</code> | <code>TAB[𝑥]\[<var>p1</var><sub>i32</sub> : <var>p3</var><sub>i32</sub>] ← <var>p2</var></code> |
| <code>3</code> | | <code>table\.copy</code> | <code>tableidx,tableidx</code> | <code>TAB[𝑥]\[<var>p1</var><sub>i32</sub> : <var>p3</var><sub>i32</sub>] ← TAB[𝑦]\[<var>p2</var><sub>i32</sub> : <var>p3</var><sub>i32</sub>]</code> |
| <code>3</code> | | <code>table\.init</code> | <code>tableidx,elemidx</code> | <code>TAB[𝑥]\[<var>p1</var><sub>i32</sub> : <var>p3</var><sub>i32</sub>] ← MEM[𝑦]\[<var>p2</var><sub>i32</sub> : <var>p3</var><sub>i32</sub>]</code> |

| 引数 | 返値 | 命令 | OP<code>𝑥</code> | 備考 |
|:-:|:-:|:--|:--|:--|
| <code>0</code> | | <code>elem\.drop</code> | <code>elemidx</code> | <code>ELEM[𝑥]</code> の要素を破棄 |


## メモリ領域操作

| 引数 | 返値 | 命令 | OP<code>𝑥</code> | 備考 |
|:-:|:-:|:--|:--|:--|
| <code>0</code> | <code>i32</code> | <code>memory\.size</code> | | <code><var>r</var><sub>i32</sub> ← (MEM0 のページ サイズ)</code> |
| <code>1</code> | <code>i32</code> | <code>memory\.grow</code> | | <code><var>r</var><sub>i32</sub> ← (MEM0 のページ サイズ)</code> <br> <code>MEM0</code> のページをサイズ <code><var>p1</var><sub>i32</sub></code> に変更 |
| <code>3</code> | | <code>memory\.fill</code> | | <code>memset(&amp;MEM0[<var>p1</var><sub>i32</sub>], <var>p2</var><sub>i32</sub>, <var>p3</var><sub>i32</sub>)</code> <br/> 位置<code>p1</code>から長さ<code>p3</code>を<code>p2</code>で埋める |
| <code>3</code> | | <code>memory\.copy</code> | | <code>memmove(&amp;MEM0[<var>p1</var><sub>i32</sub>], &amp;MEM0[<var>p2</var><sub>i32</sub>], <var>p3</var><sub>i32</sub>)</code> <br/> 位置<code>p2</code>から長さ<code>p3</code>を位置<code>p1</code>に移動 |
| <code>3</code> | | <code>memory\.init</code> | <code>dataidx</code> | <code>memcpy(&amp;MEM0[<var>p1</var><sub>i32</sub>], &amp;DATA[𝑥][<var>p2</var><sub>i32</sub>], <var>p3</var><sub>i32</sub>)</code> <br/> <cod>DATA</code>の位置<code>p2</code>から長さ<code>p3</code>を<code>MEM0</code>の位置<code>p1</code>へコピー |

<code>memset, mommove, memcpy</code>はC言語の標準Cライブラリを参照してください。

| 引数 | 返値 | 命令 | OP | 備考 |
|:-:|:-:|:--|:--|:--|
| <code>0</code> | | <code>data\.drop</code> | <code>dataidx</code> | <code>DATA[𝑥]</code> を破棄 |


## ブロック指定と制御命令

| 引数 | 返値 | 命令 | OP | 備考 |
|:-:|:-:|:--|:--|:--|
| <code>?</code> | | <code>block</code> | <code>blocktype</code> | ブロックの開始 |
| <code>?</code> | | <code>loop</code> | <code>blocktype</code> | ループ・ブロックの開始 |
| <code>1</code> | | <code>if</code> | <code>blocktype</code> | <code>if</code>ブロックの開始 |
| <code>0</code> | | <code>else</code> | | <code>else</code>ブロックの開始 |
| <code>?</code> | | <code>end</code><br/><code>end.block</code><br/><code>end.loop</code><br/><code>end.if</code> | | ブロックまたはコードの終端 <br/>(別名は入れ子対応表記用)<br/>&nbsp;<br/>&nbsp; |
| | | | | |
| <code>0</code> | | <code>br</code> | <code>labelidx</code> | ブロックからの脱出<br/><code>labelidx</code>は<code>ブロック段数-1</code></code> |
| <code>1</code> | | <code>br\_if</code> | <code>labelidx</code> | <code><var>p1</var><sub>i32</sub></code>が真なら<br/>&nbsp;&nbsp;&nbsp;&nbsp;<code>br  labelidx</code><br/>が実行される |
| <code>1</code> | | <code>br\_table</code> | <code>labelidx,labelidx,...</code> | <code>br  [...labelidx]\[<var>p1</var><sub>i32</sub>]</code> |
| <code>?</code> | | <code>return</code> | | 関数から戻る |
| <code>?</code> | <code>?</code> | <code>call</code> | <code>funcidx</code> | 関数を呼び出す |
| <code>1+?</code> | <code>?</code> | <code>call\_indirect</code> | <code>tableidx,typeidx</code> | <code>typeidx</code>型の関数<br/>&nbsp;&nbsp;&nbsp;&nbsp;<code>TAB[tableidx]\[<var>p1</var><sub>i32</sub>]</code><br/>を呼び出す |
| | | | | |
| <code>0</code> | | <code>unreachable</code> | | 不到達(<code>trap</code>発生) |
| <code>0</code> | | <code>nop</code> | | 何もしない |


# ブロック

以下の表は処理順を表します。ブロック内の<code>br</code>分岐命令の行き先は次の通り。

### block 〜 end

| <code>br</code> | コード | 備考 |
|:-:|:--|:--|
| | <code>block <var>t</var></code> | ブロックの開始 |
| ↓ | <code>𝑖𝑛𝑠𝑡𝑟...</code> | ブロック内のコード |
| ✓ | <code>end</code>  | ブロックの終了 |


### loop 〜 end

| <code>br</code> | コード | 備考 |
|:-:|:--|:--|
| ✓ | <code>loop <var>t</var></code> | ブロックの開始 |
| ↑ | <code>𝑖𝑛𝑠𝑡𝑟...</code> | ブロック内のコード |
| | <code>end</code> | ブロックの終了 |


### if 〜 else 〜 end

| <code>br</code> | コード | 備考 |
|:-:|:--|:--|
| | <code>if <var>t</var></code> | 引数<code><var>p1</var><sub>i32</sub></code>が偽ならば<code>else</code>の次へ |
| ↓ | <code>𝑖𝑛𝑠𝑡𝑟...</code> | 下記に相当<br/><code>block <var>t</var></code><br/><code>𝑖𝑛𝑠𝑡𝑟...</code><br/><code>end</code> |
| ✓ | <code>else</code> | <code>end</code>へジャンプ |
| ↓ | <code>𝑖𝑛𝑠𝑡𝑟...</code> | 下記に相当<br/><code>block <var>t</var></code><br/><code>𝑖𝑛𝑠𝑡𝑟...</code><br/><code>end</code> |
| ✓ | <code>end</code> | |
