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


## コメント

<code>#</code> または <code>;</code> から行末まで無視されます。

```
# コメント1↲
; コメント2↲
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


## 文字列

文字<code>"</code>または<code>'</code>で括られた範囲を文字列とします。文字<code>\\</code>をエスケープ文字とします。

| 種類 | エスケープ シーケンス |
|:--|:--|
| 制御文字 | <code>\\a</code> <code>\\b</code> <code>\\t</code> <code>\\n</code> <code>\\v</code> <code>\\r</code> |
| 8進コード | 最大3桁(8bit) : <code>\\</code>NNN |
| 16進コード | 最大2桁(8bit) : <code>\\x</code>HH |
| UCSコード | 最大6桁(24bit) : <code>\\u</code>HHHHHH |
| 上記以外 | <code>\\</code> の直後の文字が有効 <br/> 例: <code>\\\\</code> → <code>\\</code>  |


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
| vectype | v128 <br/> (i8x16 i16x8 i32x4 i64x2 f32x4 f64x2) |
| reftype | funcref externref |
| valtype | numtype:<br/>&nbsp;&nbsp;&nbsp; i32 i64 f32 f64 <br/> vectype:<br/>&nbsp;&nbsp;&nbsp; v128 <br/> reftype:<br/>&nbsp;&nbsp;&nbsp; funcref externref |


# 疑似命令


## <code>.alias</code>疑似命令

```
name    .alias          value
```

<code>name</code>を<code>value</code>として扱います。


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

[WebAssembly - Instructions](https://webassembly.github.io/spec/core/binary/instructions.html) を参照してください。


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

以下の命令は、<code>v128.const</code>の別名として処理します。

| 命令 | 例 |
|:-:|:--|
| <code>v128.const</code> | <code>v128.const   0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15</code> |
| <code>i8x16.const</code> | <code>i8x16.const  0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15</code> |
| <code>i16x8.const</code> | <code>i16x8.const  0x0100,0x0302,0x0504,0x0706,0x0908,0x0b0a,0x0d0c,0x0f0e</code> |
| <code>i32x4.const</code> | <code>i32x4.const  0x03020100,0x07060504,0x0b0a0908,0x0f0e0d0c</code> |
| <code>i62x2.const</code> | <code>i64x2.const  0x0706050403020100,0x0f0e0d0c0b0a0908</code> |
| <code>f32x4.const</code> | <code>f32x4.const  1.0, 2.0, 3.0, 4.0</code> |
| <code>f62x2.const</code> | <code>f64x2.const  0.5, 0.25</code> |

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


## 命令表

| 引数 | 返値 | 命令 | オペランド |
|:-:|:-:|:--|:--|
| <code>0</code> | <code>i32</code> | <code>i32\.const</code> | <code>i32</code> |
| <code>0</code> | <code>i64</code> | <code>i64\.const</code> | <code>i64</code> |
| <code>0</code> | <code>f32</code> | <code>f32\.const</code> | <code>f32</code> |
| <code>0</code> | <code>f64</code> | <code>f64\.const</code> | <code>f64</code> |
| <code>0</code> | <code>v128</code> | <code>i8x16\.const</code> | <code>i8<sub>1</sub>,i8<sub>2</sub>,...,i8<sub>16</sub></code> |
| <code>0</code> | <code>v128</code> | <code>i16x8\.const</code> | <code>i16<sub>1</sub>,i16<sub>2</sub>,...,i16<sub>8</sub></code> |
| <code>0</code> | <code>v128</code> | <code>i32x4\.const</code> | <code>i32,i32,i32,i32</code> |
| <code>0</code> | <code>v128</code> | <code>i64x2\.const</code> | <code>i64,i64</code> |
| <code>0</code> | <code>v128</code> | <code>f32x4\.const</code> | <code>f32,f32,f32,f32</code> |
| <code>0</code> | <code>v128</code> | <code>f64x2\.const</code> | <code>f64,f64</code> |
| <code>0</code> | <code>v128</code> | <code>v128\.const</code> | <code>i8<sub>1</sub>,i8<sub>2</sub>,...,i8<sub>16</sub></code> |
| <code>1</code> |  | <code>drop</code> |  |
| <code>0</code> |  | <code>elem\.drop</code> | <code>elemidx</code> |
| <code>0</code> |  | <code>data\.drop</code> | <code>dataidx</code> |
| <code>0</code> | <code>any</code> | <code>global\.get</code> | <code>globalidx</code> |
| <code>1</code> |  | <code>global\.set</code> | <code>globalidx</code> |
| <code>1</code> |  | <code>local\.get</code> | <code>localidx</code> |
| <code>0</code> | <code>any</code> | <code>local\.set</code> | <code>localidx</code> |
| <code>1</code> | <code>any</code> | <code>local\.tee</code> | <code>localidx</code> |
| <code>1</code> | <code>any</code> | <code>table\.get</code> | <code>tableidx</code> |
| <code>2</code> |  | <code>table\.set</code> | <code>tableidx</code> |
| <code>3</code> |  | <code>table\.init</code> | <code>tableidx,elemidx</code> |
| <code>3</code> |  | <code>table\.copy</code> | <code>tableidx,tableidx</code> |
| <code>0</code> | <code>i32</code> | <code>table\.grow</code> | <code>tableidx</code> |
| <code>0</code> | <code>i32</code> | <code>table\.size</code> | <code>tableidx</code> |
| <code>3</code> |  | <code>table\.fill</code> | <code>tableidx</code> |
| <code>0</code> | <code>i32</code> | <code>memory\.size</code> |  |
| <code>1</code> | <code>i32</code> | <code>memory\.grow</code> |  |
| <code>3</code> |  | <code>memory\.init</code> | <code>dataidx</code> |
| <code>3</code> |  | <code>memory\.copy</code> |  |
| <code>3</code> |  | <code>memory\.fill</code> |  |
| <code>1</code> | <code>i32</code> | <code>i32\.load</code> | <code>&lt;memarg&gt;</code> |
| <code>1</code> | <code>i64</code> | <code>i64\.load</code> | <code>&lt;memarg&gt;</code> |
| <code>1</code> | <code>f32</code> | <code>f32\.load</code> | <code>&lt;memarg&gt;</code> |
| <code>1</code> | <code>f64</code> | <code>f64\.load</code> | <code>&lt;memarg&gt;</code> |
| <code>1</code> | <code>i32</code> | <code>i32\.load8\_s</code> | <code>&lt;memarg&gt;</code> |
| <code>1</code> | <code>i32</code> | <code>i32\.load8\_u</code> | <code>&lt;memarg&gt;</code> |
| <code>1</code> | <code>i32</code> | <code>i32\.load16\_s</code> | <code>&lt;memarg&gt;</code> |
| <code>1</code> | <code>i32</code> | <code>i32\.load16\_u</code> | <code>&lt;memarg&gt;</code> |
| <code>1</code> | <code>i64</code> | <code>i64\.load8\_s</code> | <code>&lt;memarg&gt;</code> |
| <code>1</code> | <code>i64</code> | <code>i64\.load8\_u</code> | <code>&lt;memarg&gt;</code> |
| <code>1</code> | <code>i64</code> | <code>i64\.load16\_s</code> | <code>&lt;memarg&gt;</code> |
| <code>1</code> | <code>i64</code> | <code>i64\.load16\_u</code> | <code>&lt;memarg&gt;</code> |
| <code>1</code> | <code>i64</code> | <code>i64\.load32\_s</code> | <code>&lt;memarg&gt;</code> |
| <code>1</code> | <code>i64</code> | <code>i64\.load32\_u</code> | <code>&lt;memarg&gt;</code> |
| <code>2</code> |  | <code>i32\.store</code> | <code>&lt;memarg&gt;</code> |
| <code>2</code> |  | <code>i64\.store</code> | <code>&lt;memarg&gt;</code> |
| <code>2</code> |  | <code>f32\.store</code> | <code>&lt;memarg&gt;</code> |
| <code>2</code> |  | <code>f64\.store</code> | <code>&lt;memarg&gt;</code> |
| <code>2</code> |  | <code>i32\.store8</code> | <code>&lt;memarg&gt;</code> |
| <code>2</code> |  | <code>i32\.store16</code> | <code>&lt;memarg&gt;</code> |
| <code>2</code> |  | <code>i64\.store8</code> | <code>&lt;memarg&gt;</code> |
| <code>2</code> |  | <code>i64\.store16</code> | <code>&lt;memarg&gt;</code> |
| <code>2</code> |  | <code>i64\.store32</code> | <code>&lt;memarg&gt;</code> |
| <code>1</code> | <code>v128</code> | <code>v128\.load</code> | <code>&lt;memarg&gt;</code> |
| <code>1</code> | <code>v128</code> | <code>v128\.load8x8\_s</code> | <code>&lt;memarg&gt;</code> |
| <code>1</code> | <code>v128</code> | <code>v128\.load8x8\_u</code> | <code>&lt;memarg&gt;</code> |
| <code>1</code> | <code>v128</code> | <code>v128\.load16x4\_s</code> | <code>&lt;memarg&gt;</code> |
| <code>1</code> | <code>v128</code> | <code>v128\.load16x4\_u</code> | <code>&lt;memarg&gt;</code> |
| <code>1</code> | <code>v128</code> | <code>v128\.load32x2\_s</code> | <code>&lt;memarg&gt;</code> |
| <code>1</code> | <code>v128</code> | <code>v128\.load32x2\_u</code> | <code>&lt;memarg&gt;</code> |
| <code>1</code> | <code>v128</code> | <code>v128\.load8\_splat</code> | <code>&lt;memarg&gt;</code> |
| <code>1</code> | <code>v128</code> | <code>v128\.load16\_splat</code> | <code>&lt;memarg&gt;</code> |
| <code>1</code> | <code>v128</code> | <code>v128\.load32\_splat</code> | <code>&lt;memarg&gt;</code> |
| <code>1</code> | <code>v128</code> | <code>v128\.load64\_splat</code> | <code>&lt;memarg&gt;</code> |
| <code>1</code> | <code>v128</code> | <code>v128\.store</code> | <code>&lt;memarg&gt;</code> |
| <code>2</code> | <code>v128</code> | <code>v128\.load8\_lane</code> | <code>lane,&lt;memarg&gt;</code> |
| <code>2</code> | <code>v128</code> | <code>v128\.load16\_lane</code> | <code>lane,&lt;memarg&gt;</code> |
| <code>2</code> | <code>v128</code> | <code>v128\.load32\_lane</code> | <code>lane,&lt;memarg&gt;</code> |
| <code>2</code> | <code>v128</code> | <code>v128\.load64\_lane</code> | <code>lane,&lt;memarg&gt;</code> |
| <code>2</code> |  | <code>v128\.store8\_lane</code> | <code>lane,&lt;memarg&gt;</code> |
| <code>2</code> |  | <code>v128\.store16\_lane</code> | <code>lane,&lt;memarg&gt;</code> |
| <code>2</code> |  | <code>v128\.store32\_lane</code> | <code>lane,&lt;memarg&gt;</code> |
| <code>2</code> |  | <code>v128\.store64\_lane</code> | <code>lane,&lt;memarg&gt;</code> |
| <code>1</code> | <code>v128</code> | <code>v128\.load32\_zero</code> | <code>&lt;memarg&gt;</code> |
| <code>1</code> | <code>v128</code> | <code>v128\.load64\_zero</code> | <code>&lt;memarg&gt;</code> |
| <code>1</code> | <code>i32</code> | <code>i32\.extend8\_s</code> |  |
| <code>1</code> | <code>i64</code> | <code>i32\.extend16\_s</code> |  |
| <code>1</code> | <code>i64</code> | <code>i64\.extend8\_s</code> |  |
| <code>1</code> | <code>i64</code> | <code>i64\.extend16\_s</code> |  |
| <code>1</code> | <code>i64</code> | <code>i64\.extend32\_s</code> |  |
| <code>1</code> | <code>i32</code> | <code>i32\.wrap</code> |  |
| <code>1</code> | <code>i64</code> | <code>i64\.extend\_i32\_s</code> |  |
| <code>1</code> | <code>i64</code> | <code>i64\.extend\_i32\_u</code> |  |
| <code>1</code> | <code>i32</code> | <code>i32\.trunc\_f32\_s</code> |  |
| <code>1</code> | <code>i32</code> | <code>i32\.trunc\_f32\_u</code> |  |
| <code>1</code> | <code>i32</code> | <code>i32\.trunc\_f64\_s</code> |  |
| <code>1</code> | <code>i32</code> | <code>i32\.trunc\_f64\_u</code> |  |
| <code>1</code> | <code>i64</code> | <code>i64\.trunc\_f32\_s</code> |  |
| <code>1</code> | <code>i64</code> | <code>i64\.trunc\_f32\_u</code> |  |
| <code>1</code> | <code>i64</code> | <code>i64\.trunc\_f64\_s</code> |  |
| <code>1</code> | <code>i64</code> | <code>i64\.trunc\_f64\_u</code> |  |
| <code>1</code> | <code>i32</code> | <code>i32\.trunc\_sat\_f32\_s</code> |  |
| <code>1</code> | <code>i32</code> | <code>i32\.trunc\_sat\_f32\_u</code> |  |
| <code>1</code> | <code>i32</code> | <code>i32\.trunc\_sat\_f64\_s</code> |  |
| <code>1</code> | <code>i32</code> | <code>i32\.trunc\_sat\_f64\_u</code> |  |
| <code>1</code> | <code>i64</code> | <code>i64\.trunc\_sat\_f32\_s</code> |  |
| <code>1</code> | <code>i64</code> | <code>i64\.trunc\_sat\_f32\_u</code> |  |
| <code>1</code> | <code>i64</code> | <code>i64\.trunc\_sat\_f64\_s</code> |  |
| <code>1</code> | <code>i64</code> | <code>i64\.trunc\_sat\_f64\_u</code> |  |
| <code>1</code> | <code>f32</code> | <code>f32\.demote\_f64</code> |  |
| <code>1</code> | <code>f64</code> | <code>f64\.promote\_f32</code> |  |
| <code>1</code> | <code>f32</code> | <code>f32\.convert\_i32\_s</code> |  |
| <code>1</code> | <code>f32</code> | <code>f32\.convert\_i32\_u</code> |  |
| <code>1</code> | <code>f32</code> | <code>f32\.convert\_i64\_s</code> |  |
| <code>1</code> | <code>f32</code> | <code>f32\.convert\_i64\_u</code> |  |
| <code>1</code> | <code>f64</code> | <code>f64\.convert\_i32\_s</code> |  |
| <code>1</code> | <code>f64</code> | <code>f64\.convert\_i32\_u</code> |  |
| <code>1</code> | <code>f64</code> | <code>f64\.convert\_i64\_s</code> |  |
| <code>1</code> | <code>f64</code> | <code>f64\.convert\_i64\_u</code> |  |
| <code>1</code> | <code>i32</code> | <code>i32\.reinterpret\_f32</code> |  |
| <code>1</code> | <code>i64</code> | <code>i64\.reinterpret\_f64</code> |  |
| <code>1</code> | <code>f32</code> | <code>f32\.reinterpret\_i32</code> |  |
| <code>1</code> | <code>f64</code> | <code>f64\.reinterpret\_i64</code> |  |
| <code>1</code> | <code>v128</code> | <code>i8x16\.splat</code> |  |
| <code>1</code> | <code>v128</code> | <code>i16x8\.splat</code> |  |
| <code>1</code> | <code>v128</code> | <code>i32x4\.splat</code> |  |
| <code>1</code> | <code>v128</code> | <code>i64x2\.splat</code> |  |
| <code>1</code> | <code>v128</code> | <code>f32x4\.splat</code> |  |
| <code>1</code> | <code>v128</code> | <code>f64x2\.splat</code> |  |
| <code>1</code> | <code>i32</code> | <code>i8x16\.extract\_lane\_s</code> | <code>lane</code> |
| <code>1</code> | <code>i32</code> | <code>i8x16\.extract\_lane\_u</code> | <code>lane</code> |
| <code>1</code> | <code>i32</code> | <code>i16x8\.extract\_lane\_s</code> | <code>lane</code> |
| <code>1</code> | <code>i32</code> | <code>i16x8\.extract\_lane\_u</code> | <code>lane</code> |
| <code>1</code> | <code>i32</code> | <code>i32x4\.extract\_lane</code> | <code>lane</code> |
| <code>1</code> | <code>i64</code> | <code>i64x2\.extract\_lane</code> | <code>lane</code> |
| <code>1</code> | <code>f32</code> | <code>f32x4\.extract\_lane</code> | <code>lane</code> |
| <code>1</code> | <code>f64</code> | <code>f64x2\.extract\_lane</code> | <code>lane</code> |
| <code>1</code> | <code>v128</code> | <code>i32x4\.trunc\_sat\_f32x4\_s</code> |  |
| <code>1</code> | <code>v128</code> | <code>i32x4\.trunc\_sat\_f32x4\_u</code> |  |
| <code>1</code> | <code>v128</code> | <code>i32x4\.trunc\_sat\_f64x2\_s\_zero</code> |  |
| <code>1</code> | <code>v128</code> | <code>i32x4\.trunc\_sat\_f64x2\_u\_zero</code> |  |
| <code>1</code> | <code>v128</code> | <code>f32x4\.convert\_i32x4\_s</code> |  |
| <code>1</code> | <code>v128</code> | <code>f32x4\.convert\_i32x4\_u</code> |  |
| <code>1</code> | <code>v128</code> | <code>f64x2\.convert\_low\_i32x4\_s</code> |  |
| <code>1</code> | <code>v128</code> | <code>f64x2\.convert\_low\_i32x4\_u</code> |  |
| <code>1</code> | <code>v128</code> | <code>f32x4\.demote\_f64x2\_zero</code> |  |
| <code>1</code> | <code>v128</code> | <code>f64x2\.promote\_low\_f32x4</code> |  |
| <code>1</code> | <code>v128</code> | <code>i16x8\.extend\_low\_i8x16\_s</code> |  |
| <code>1</code> | <code>v128</code> | <code>i16x8\.extend\_high\_i8x16\_s</code> |  |
| <code>1</code> | <code>v128</code> | <code>i16x8\.extend\_low\_i8x16\_u</code> |  |
| <code>1</code> | <code>v128</code> | <code>i16x8\.extend\_high\_i8x16\_u</code> |  |
| <code>1</code> | <code>v128</code> | <code>i32x4\.extend\_low\_i16x8\_s</code> |  |
| <code>1</code> | <code>v128</code> | <code>i32x4\.extend\_high\_i16x8\_s</code> |  |
| <code>1</code> | <code>v128</code> | <code>i32x4\.extend\_low\_i16x8\_u</code> |  |
| <code>1</code> | <code>v128</code> | <code>i32x4\.extend\_high\_i16x8\_u</code> |  |
| <code>1</code> | <code>v128</code> | <code>i64x2\.extend\_low\_i32x4\_s</code> |  |
| <code>1</code> | <code>v128</code> | <code>i64x2\.extend\_high\_i32x4\_s</code> |  |
| <code>1</code> | <code>v128</code> | <code>i64x2\.extend\_low\_i32x4\_u</code> |  |
| <code>1</code> | <code>v128</code> | <code>i64x2\.extend\_high\_i32x4\_u</code> |  |
| <code>1</code> | <code>v128</code> | <code>i8x16\.narrow\_i16x8\_s</code> |  |
| <code>1</code> | <code>v128</code> | <code>i8x16\.narrow\_i16x8\_u</code> |  |
| <code>1</code> | <code>v128</code> | <code>i16x8\.narrow\_i32x4\_s</code> |  |
| <code>1</code> | <code>v128</code> | <code>i16x8\.narrow\_i32x4\_u</code> |  |
| <code>1</code> | <code>v128</code> | <code>i32x4\.narrow\_i32x4\_s</code> |  |
| <code>1</code> | <code>v128</code> | <code>i32x4\.narrow\_i32x4\_u</code> |  |
| <code>2</code> | <code>v128</code> | <code>i8x16\.replace\_lane</code> | <code>lane</code> |
| <code>2</code> | <code>v128</code> | <code>i16x8\.replace\_lane</code> | <code>lane</code> |
| <code>2</code> | <code>v128</code> | <code>i32x4\.replace\_lane</code> | <code>lane</code> |
| <code>2</code> | <code>v128</code> | <code>i64x2\.replace\_lane</code> | <code>lane</code> |
| <code>2</code> | <code>v128</code> | <code>f32x4\.replace\_lane</code> | <code>lane</code> |
| <code>2</code> | <code>v128</code> | <code>f64x2\.replace\_lane</code> | <code>lane</code> |
| <code>2</code> | <code>v128</code> | <code>i8x16\.shuffle</code> | <code>lane<sub>1</sub>,...,lane<sub>16</sub></code> |
| <code>2</code> | <code>v128</code> | <code>i8x16\.swizzle</code> |  |
| <code>1</code> | <code>i32</code> | <code>i32\.eqz</code> |  |
| <code>2</code> | <code>i32</code> | <code>i32\.eq</code> |  |
| <code>2</code> | <code>i32</code> | <code>i32\.ne</code> |  |
| <code>2</code> | <code>i32</code> | <code>i32\.lt\_s</code> |  |
| <code>2</code> | <code>i32</code> | <code>i32\.lt\_u</code> |  |
| <code>2</code> | <code>i32</code> | <code>i32\.gt\_s</code> |  |
| <code>2</code> | <code>i32</code> | <code>i32\.gt\_u</code> |  |
| <code>2</code> | <code>i32</code> | <code>i32\.le\_s</code> |  |
| <code>2</code> | <code>i32</code> | <code>i32\.le\_u</code> |  |
| <code>2</code> | <code>i32</code> | <code>i32\.ge\_s</code> |  |
| <code>2</code> | <code>i32</code> | <code>i32\.ge\_u</code> |  |
| <code>1</code> | <code>i32</code> | <code>i64\.eqz</code> |  |
| <code>2</code> | <code>i32</code> | <code>i64\.eq</code> |  |
| <code>2</code> | <code>i32</code> | <code>i64\.ne</code> |  |
| <code>2</code> | <code>i32</code> | <code>i64\.lt\_s</code> |  |
| <code>2</code> | <code>i32</code> | <code>i64\.lt\_u</code> |  |
| <code>2</code> | <code>i32</code> | <code>i64\.gt\_s</code> |  |
| <code>2</code> | <code>i32</code> | <code>i64\.gt\_u</code> |  |
| <code>2</code> | <code>i32</code> | <code>i64\.le\_s</code> |  |
| <code>2</code> | <code>i32</code> | <code>i64\.le\_u</code> |  |
| <code>2</code> | <code>i32</code> | <code>i64\.ge\_s</code> |  |
| <code>2</code> | <code>i32</code> | <code>i64\.ge\_u</code> |  |
| <code>2</code> | <code>i32</code> | <code>f32\.eq</code> |  |
| <code>2</code> | <code>i32</code> | <code>f32\.ne</code> |  |
| <code>2</code> | <code>i32</code> | <code>f32\.lt</code> |  |
| <code>2</code> | <code>i32</code> | <code>f32\.gt</code> |  |
| <code>2</code> | <code>i32</code> | <code>f32\.le</code> |  |
| <code>2</code> | <code>i32</code> | <code>f32\.ge</code> |  |
| <code>2</code> | <code>i32</code> | <code>f64\.eq</code> |  |
| <code>2</code> | <code>i32</code> | <code>f64\.ne</code> |  |
| <code>2</code> | <code>i32</code> | <code>f64\.lt</code> |  |
| <code>2</code> | <code>i32</code> | <code>f64\.gt</code> |  |
| <code>2</code> | <code>i32</code> | <code>f64\.le</code> |  |
| <code>2</code> | <code>i32</code> | <code>f64\.ge</code> |  |
| <code>2</code> | <code>v128</code> | <code>i8x16\.eq</code> |  |
| <code>2</code> | <code>v128</code> | <code>i8x16\.ne</code> |  |
| <code>2</code> | <code>v128</code> | <code>i8x16\.lt\_s</code> |  |
| <code>2</code> | <code>v128</code> | <code>i8x16\.lt\_u</code> |  |
| <code>2</code> | <code>v128</code> | <code>i8x16\.gt\_s</code> |  |
| <code>2</code> | <code>v128</code> | <code>i8x16\.gt\_u</code> |  |
| <code>2</code> | <code>v128</code> | <code>i8x16\.le\_s</code> |  |
| <code>2</code> | <code>v128</code> | <code>i8x16\.le\_u</code> |  |
| <code>2</code> | <code>v128</code> | <code>i8x16\.ge\_s</code> |  |
| <code>2</code> | <code>v128</code> | <code>i8x16\.ge\_u</code> |  |
| <code>2</code> | <code>v128</code> | <code>i16x8\.eq</code> |  |
| <code>2</code> | <code>v128</code> | <code>i16x8\.ne</code> |  |
| <code>2</code> | <code>v128</code> | <code>i16x8\.lt\_s</code> |  |
| <code>2</code> | <code>v128</code> | <code>i16x8\.lt\_u</code> |  |
| <code>2</code> | <code>v128</code> | <code>i16x8\.gt\_s</code> |  |
| <code>2</code> | <code>v128</code> | <code>i16x8\.gt\_u</code> |  |
| <code>2</code> | <code>v128</code> | <code>i16x8\.le\_s</code> |  |
| <code>2</code> | <code>v128</code> | <code>i16x8\.le\_u</code> |  |
| <code>2</code> | <code>v128</code> | <code>i16x8\.ge\_s</code> |  |
| <code>2</code> | <code>v128</code> | <code>i16x8\.ge\_u</code> |  |
| <code>2</code> | <code>v128</code> | <code>i32x4\.eq</code> |  |
| <code>2</code> | <code>v128</code> | <code>i32x4\.ne</code> |  |
| <code>2</code> | <code>v128</code> | <code>i32x4\.lt\_s</code> |  |
| <code>2</code> | <code>v128</code> | <code>i32x4\.lt\_u</code> |  |
| <code>2</code> | <code>v128</code> | <code>i32x4\.gt\_s</code> |  |
| <code>2</code> | <code>v128</code> | <code>i32x4\.gt\_u</code> |  |
| <code>2</code> | <code>v128</code> | <code>i32x4\.le\_s</code> |  |
| <code>2</code> | <code>v128</code> | <code>i32x4\.le\_u</code> |  |
| <code>2</code> | <code>v128</code> | <code>i32x4\.ge\_s</code> |  |
| <code>2</code> | <code>v128</code> | <code>i32x4\.ge\_u</code> |  |
| <code>2</code> | <code>v128</code> | <code>i64x2\.eq</code> |  |
| <code>2</code> | <code>v128</code> | <code>i64x2\.ne</code> |  |
| <code>2</code> | <code>v128</code> | <code>i64x2\.lt\_s</code> |  |
| <code>2</code> | <code>v128</code> | <code>i64x2\.gt\_s</code> |  |
| <code>2</code> | <code>v128</code> | <code>i64x2\.le\_s</code> |  |
| <code>2</code> | <code>v128</code> | <code>i64x2\.ge\_s</code> |  |
| <code>2</code> | <code>v128</code> | <code>f32x4\.eq</code> |  |
| <code>2</code> | <code>v128</code> | <code>f32x4\.ne</code> |  |
| <code>2</code> | <code>v128</code> | <code>f32x4\.lt</code> |  |
| <code>2</code> | <code>v128</code> | <code>f32x4\.gt</code> |  |
| <code>2</code> | <code>v128</code> | <code>f32x4\.le</code> |  |
| <code>2</code> | <code>v128</code> | <code>f32x4\.ge</code> |  |
| <code>2</code> | <code>v128</code> | <code>f64x2\.eq</code> |  |
| <code>2</code> | <code>v128</code> | <code>f64x2\.ne</code> |  |
| <code>2</code> | <code>v128</code> | <code>f64x2\.lt</code> |  |
| <code>2</code> | <code>v128</code> | <code>f64x2\.gt</code> |  |
| <code>2</code> | <code>v128</code> | <code>f64x2\.le</code> |  |
| <code>2</code> | <code>v128</code> | <code>f64x2\.ge</code> |  |
| <code>1</code> | <code>i32</code> | <code>v128\.any\_true</code> |  |
| <code>1</code> | <code>i32</code> | <code>i8x16\.all\_true</code> |  |
| <code>1</code> | <code>i32</code> | <code>i16x8\.all\_true</code> |  |
| <code>1</code> | <code>i32</code> | <code>i32x4\.all\_true</code> |  |
| <code>1</code> | <code>i32</code> | <code>i64x2\.all\_true</code> |  |
| <code>3</code> | <code>any</code> | <code>select</code> |  |
| <code>3</code> | <code>any</code> | <code>select</code> | <code>valtype,valtype,...</code> |
| <code>1</code> | <code>i32</code> | <code>i32\.clz</code> |  |
| <code>1</code> | <code>i32</code> | <code>i32\.ctz</code> |  |
| <code>1</code> | <code>i32</code> | <code>i32\.popcnt</code> |  |
| <code>2</code> | <code>i32</code> | <code>i32\.add</code> |  |
| <code>2</code> | <code>i32</code> | <code>i32\.sub</code> |  |
| <code>2</code> | <code>i32</code> | <code>i32\.mul</code> |  |
| <code>2</code> | <code>i32</code> | <code>i32\.div\_s</code> |  |
| <code>2</code> | <code>i32</code> | <code>i32\.div\_u</code> |  |
| <code>2</code> | <code>i32</code> | <code>i32\.rem\_s</code> |  |
| <code>2</code> | <code>i32</code> | <code>i32\.rem\_u</code> |  |
| <code>2</code> | <code>i32</code> | <code>i32\.and</code> |  |
| <code>2</code> | <code>i32</code> | <code>i32\.or</code> |  |
| <code>2</code> | <code>i32</code> | <code>i32\.xor</code> |  |
| <code>2</code> | <code>i32</code> | <code>i32\.shl</code> |  |
| <code>2</code> | <code>i32</code> | <code>i32\.shr\_s</code> |  |
| <code>2</code> | <code>i32</code> | <code>i32\.shr\_u</code> |  |
| <code>2</code> | <code>i32</code> | <code>i32\.rotl</code> |  |
| <code>2</code> | <code>i32</code> | <code>i32\.rotr</code> |  |
| <code>1</code> | <code>i64</code> | <code>i64\.clz</code> |  |
| <code>1</code> | <code>i64</code> | <code>i64\.ctz</code> |  |
| <code>1</code> | <code>i64</code> | <code>i64\.popcnt</code> |  |
| <code>2</code> | <code>i64</code> | <code>i64\.add</code> |  |
| <code>2</code> | <code>i64</code> | <code>i64\.sub</code> |  |
| <code>2</code> | <code>i64</code> | <code>i64\.mul</code> |  |
| <code>2</code> | <code>i64</code> | <code>i64\.div\_s</code> |  |
| <code>2</code> | <code>i64</code> | <code>i64\.div\_u</code> |  |
| <code>2</code> | <code>i64</code> | <code>i64\.rem\_s</code> |  |
| <code>2</code> | <code>i64</code> | <code>i64\.rem\_u</code> |  |
| <code>2</code> | <code>i64</code> | <code>i64\.and</code> |  |
| <code>2</code> | <code>i64</code> | <code>i64\.or</code> |  |
| <code>2</code> | <code>i64</code> | <code>i64\.xor</code> |  |
| <code>2</code> | <code>i64</code> | <code>i64\.shl</code> |  |
| <code>2</code> | <code>i64</code> | <code>i64\.shr\_s</code> |  |
| <code>2</code> | <code>i64</code> | <code>i64\.shr\_u</code> |  |
| <code>2</code> | <code>i64</code> | <code>i64\.rotl</code> |  |
| <code>2</code> | <code>i64</code> | <code>i64\.rotr</code> |  |
| <code>1</code> | <code>f32</code> | <code>f32\.abs</code> |  |
| <code>1</code> | <code>f32</code> | <code>f32\.neg</code> |  |
| <code>1</code> | <code>f32</code> | <code>f32\.ceil</code> |  |
| <code>1</code> | <code>f32</code> | <code>f32\.floor</code> |  |
| <code>1</code> | <code>f32</code> | <code>f32\.trunc</code> |  |
| <code>1</code> | <code>f32</code> | <code>f32\.nearest</code> |  |
| <code>1</code> | <code>f32</code> | <code>f32\.sqrt</code> |  |
| <code>2</code> | <code>f32</code> | <code>f32\.add</code> |  |
| <code>2</code> | <code>f32</code> | <code>f32\.sub</code> |  |
| <code>2</code> | <code>f32</code> | <code>f32\.mul</code> |  |
| <code>2</code> | <code>f32</code> | <code>f32\.div</code> |  |
| <code>2</code> | <code>f32</code> | <code>f32\.min</code> |  |
| <code>2</code> | <code>f32</code> | <code>f32\.max</code> |  |
| <code>2</code> | <code>f32</code> | <code>f32\.copysign</code> |  |
| <code>1</code> | <code>f64</code> | <code>f64\.abs</code> |  |
| <code>1</code> | <code>f64</code> | <code>f64\.neg</code> |  |
| <code>1</code> | <code>f64</code> | <code>f64\.ceil</code> |  |
| <code>1</code> | <code>f64</code> | <code>f64\.floor</code> |  |
| <code>1</code> | <code>f64</code> | <code>f64\.trunc</code> |  |
| <code>1</code> | <code>f64</code> | <code>f64\.nearest</code> |  |
| <code>1</code> | <code>f64</code> | <code>f64\.sqrt</code> |  |
| <code>2</code> | <code>f64</code> | <code>f64\.add</code> |  |
| <code>2</code> | <code>f64</code> | <code>f64\.sub</code> |  |
| <code>2</code> | <code>f64</code> | <code>f64\.mul</code> |  |
| <code>2</code> | <code>f64</code> | <code>f64\.div</code> |  |
| <code>2</code> | <code>f64</code> | <code>f64\.min</code> |  |
| <code>2</code> | <code>f64</code> | <code>f64\.max</code> |  |
| <code>2</code> | <code>f64</code> | <code>f64\.copysign</code> |  |
| <code>1</code> | <code>v128</code> | <code>v128\.not</code> |  |
| <code>2</code> | <code>v128</code> | <code>v128\.and</code> |  |
| <code>2</code> | <code>v128</code> | <code>v128\.andnot</code> |  |
| <code>2</code> | <code>v128</code> | <code>v128\.or</code> |  |
| <code>2</code> | <code>v128</code> | <code>v128\.xor</code> |  |
| <code>3</code> | <code>v128</code> | <code>v128\.bitselect</code> |  |
| <code>1</code> | <code>v128</code> | <code>i8x16\.abs</code> |  |
| <code>1</code> | <code>v128</code> | <code>i8x16\.neg</code> |  |
| <code>1</code> | <code>v128</code> | <code>i8x16\.popcnt</code> |  |
| <code>1</code> | <code>i32</code> | <code>i8x16\.bitmask</code> |  |
| <code>2</code> | <code>v128</code> | <code>i8x16\.shl</code> |  |
| <code>2</code> | <code>v128</code> | <code>i8x16\.shr\_s</code> |  |
| <code>2</code> | <code>v128</code> | <code>i8x16\.shr\_u</code> |  |
| <code>2</code> | <code>v128</code> | <code>i8x16\.add</code> |  |
| <code>2</code> | <code>v128</code> | <code>i8x16\.add\_sat\_s</code> |  |
| <code>2</code> | <code>v128</code> | <code>i8x16\.add\_sat\_u</code> |  |
| <code>2</code> | <code>v128</code> | <code>i8x16\.sub</code> |  |
| <code>2</code> | <code>v128</code> | <code>i8x16\.sub\_sat\_s</code> |  |
| <code>2</code> | <code>v128</code> | <code>i8x16\.sub\_sat\_u</code> |  |
| <code>2</code> | <code>v128</code> | <code>i8x16\.min\_s</code> |  |
| <code>2</code> | <code>v128</code> | <code>i8x16\.min\_u</code> |  |
| <code>2</code> | <code>v128</code> | <code>i8x16\.max\_s</code> |  |
| <code>2</code> | <code>v128</code> | <code>i8x16\.max\_u</code> |  |
| <code>2</code> | <code>v128</code> | <code>i8x16\.avr\_u</code> |  |
| <code>1</code> | <code>v128</code> | <code>i16x8\.abs</code> |  |
| <code>1</code> | <code>v128</code> | <code>i16x8\.neg</code> |  |
| <code>1</code> | <code>i32</code> | <code>i16x8\.bitmask</code> |  |
| <code>2</code> | <code>v128</code> | <code>i16x8\.shl</code> |  |
| <code>2</code> | <code>v128</code> | <code>i16x8\.shr\_s</code> |  |
| <code>2</code> | <code>v128</code> | <code>i16x8\.shr\_u</code> |  |
| <code>2</code> | <code>v128</code> | <code>i16x8\.add</code> |  |
| <code>2</code> | <code>v128</code> | <code>i16x8\.add\_sat\_s</code> |  |
| <code>2</code> | <code>v128</code> | <code>i16x8\.add\_sat\_u</code> |  |
| <code>2</code> | <code>v128</code> | <code>i16x8\.sub</code> |  |
| <code>2</code> | <code>v128</code> | <code>i16x8\.sub\_sat\_s</code> |  |
| <code>2</code> | <code>v128</code> | <code>i16x8\.sub\_sat\_u</code> |  |
| <code>2</code> | <code>v128</code> | <code>i16x8\.mul</code> |  |
| <code>2</code> | <code>v128</code> | <code>i16x8\.min\_s</code> |  |
| <code>2</code> | <code>v128</code> | <code>i16x8\.min\_u</code> |  |
| <code>2</code> | <code>v128</code> | <code>i16x8\.max\_s</code> |  |
| <code>2</code> | <code>v128</code> | <code>i16x8\.max\_u</code> |  |
| <code>2</code> | <code>v128</code> | <code>i16x8\.avr\_u</code> |  |
| <code>2</code> | <code>v128</code> | <code>i16x8\.q15mulr\_sat\_s</code> |  |
| <code>2</code> | <code>v128</code> | <code>i16x8\.extadd\_pairwise\_i8x16\_s</code> |  |
| <code>2</code> | <code>v128</code> | <code>i16x8\.extadd\_pairwise\_i8x16\_u</code> |  |
| <code>2</code> | <code>v128</code> | <code>i16x8\.extmul\_low\_i8x16\_s</code> |  |
| <code>2</code> | <code>v128</code> | <code>i16x8\.extmul\_high\_i8x16\_s</code> |  |
| <code>2</code> | <code>v128</code> | <code>i16x8\.extmul\_low\_i8x16\_u</code> |  |
| <code>2</code> | <code>v128</code> | <code>i16x8\.extmul\_high\_i8x16\_u</code> |  |
| <code>1</code> | <code>v128</code> | <code>i32x4\.abs</code> |  |
| <code>1</code> | <code>v128</code> | <code>i32x4\.neg</code> |  |
| <code>1</code> | <code>i32</code> | <code>i32x4\.bitmask</code> |  |
| <code>2</code> | <code>v128</code> | <code>i32x4\.shl</code> |  |
| <code>2</code> | <code>v128</code> | <code>i32x4\.shr\_s</code> |  |
| <code>2</code> | <code>v128</code> | <code>i32x4\.shr\_u</code> |  |
| <code>2</code> | <code>v128</code> | <code>i32x4\.add</code> |  |
| <code>2</code> | <code>v128</code> | <code>i32x4\.sub</code> |  |
| <code>2</code> | <code>v128</code> | <code>i32x4\.mul</code> |  |
| <code>2</code> | <code>v128</code> | <code>i32x4\.min\_s</code> |  |
| <code>2</code> | <code>v128</code> | <code>i32x4\.min\_u</code> |  |
| <code>2</code> | <code>v128</code> | <code>i32x4\.max\_s</code> |  |
| <code>2</code> | <code>v128</code> | <code>i32x4\.max\_u</code> |  |
| <code>2</code> | <code>v128</code> | <code>i32x4\.dot\_i16x8\_s</code> |  |
| <code>2</code> | <code>v128</code> | <code>i32x4\.extadd\_pairwise\_i16x8\_s</code> |  |
| <code>2</code> | <code>v128</code> | <code>i32x4\.extadd\_pairwise\_i16x8\_u</code> |  |
| <code>2</code> | <code>v128</code> | <code>i32x4\.extmul\_low\_i16x8\_s</code> |  |
| <code>2</code> | <code>v128</code> | <code>i32x4\.extmul\_high\_i16x8\_s</code> |  |
| <code>2</code> | <code>v128</code> | <code>i32x4\.extmul\_low\_i16x8\_u</code> |  |
| <code>2</code> | <code>v128</code> | <code>i32x4\.extmul\_high\_i16x8\_u</code> |  |
| <code>1</code> | <code>v128</code> | <code>i64x2\.abs</code> |  |
| <code>1</code> | <code>v128</code> | <code>i64x2\.neg</code> |  |
| <code>1</code> | <code>i32</code> | <code>i64x2\.bitmask</code> |  |
| <code>2</code> | <code>v128</code> | <code>i64x2\.shl</code> |  |
| <code>2</code> | <code>v128</code> | <code>i64x2\.shr\_s</code> |  |
| <code>2</code> | <code>v128</code> | <code>i64x2\.shr\_u</code> |  |
| <code>2</code> | <code>v128</code> | <code>i64x2\.add</code> |  |
| <code>2</code> | <code>v128</code> | <code>i64x2\.sub</code> |  |
| <code>2</code> | <code>v128</code> | <code>i64x2\.mul</code> |  |
| <code>2</code> | <code>v128</code> | <code>i64x2\.extmul\_low\_i8x16\_s</code> |  |
| <code>2</code> | <code>v128</code> | <code>i64x2\.extmul\_high\_i8x16\_s</code> |  |
| <code>2</code> | <code>v128</code> | <code>i64x2\.extmul\_low\_i8x16\_u</code> |  |
| <code>2</code> | <code>v128</code> | <code>i64x2\.extmul\_high\_i8x16\_u</code> |  |
| <code>1</code> | <code>v128</code> | <code>f32x4\.ceil</code> |  |
| <code>1</code> | <code>v128</code> | <code>f32x4\.floor</code> |  |
| <code>1</code> | <code>v128</code> | <code>f32x4\.trunc</code> |  |
| <code>1</code> | <code>v128</code> | <code>f32x4\.nearest</code> |  |
| <code>1</code> | <code>v128</code> | <code>f32x4\.abs</code> |  |
| <code>1</code> | <code>v128</code> | <code>f32x4\.neg</code> |  |
| <code>1</code> | <code>v128</code> | <code>f32x4\.sqrt</code> |  |
| <code>2</code> | <code>v128</code> | <code>f32x4\.add</code> |  |
| <code>2</code> | <code>v128</code> | <code>f32x4\.sub</code> |  |
| <code>2</code> | <code>v128</code> | <code>f32x4\.mul</code> |  |
| <code>2</code> | <code>v128</code> | <code>f32x4\.div</code> |  |
| <code>2</code> | <code>v128</code> | <code>f32x4\.min</code> |  |
| <code>2</code> | <code>v128</code> | <code>f32x4\.max</code> |  |
| <code>2</code> | <code>v128</code> | <code>f32x4\.pmin</code> |  |
| <code>2</code> | <code>v128</code> | <code>f32x4\.pmax</code> |  |
| <code>1</code> | <code>v128</code> | <code>f64x2\.ceil</code> |  |
| <code>1</code> | <code>v128</code> | <code>f64x2\.floor</code> |  |
| <code>1</code> | <code>v128</code> | <code>f64x2\.trunc</code> |  |
| <code>1</code> | <code>v128</code> | <code>f64x2\.nearest</code> |  |
| <code>1</code> | <code>v128</code> | <code>f64x2\.abs</code> |  |
| <code>1</code> | <code>v128</code> | <code>f64x2\.neg</code> |  |
| <code>1</code> | <code>v128</code> | <code>f64x2\.sqrt</code> |  |
| <code>2</code> | <code>v128</code> | <code>f64x2\.add</code> |  |
| <code>2</code> | <code>v128</code> | <code>f64x2\.sub</code> |  |
| <code>2</code> | <code>v128</code> | <code>f64x2\.mul</code> |  |
| <code>2</code> | <code>v128</code> | <code>f64x2\.div</code> |  |
| <code>2</code> | <code>v128</code> | <code>f64x2\.min</code> |  |
| <code>2</code> | <code>v128</code> | <code>f64x2\.max</code> |  |
| <code>2</code> | <code>v128</code> | <code>f64x2\.pmin</code> |  |
| <code>2</code> | <code>v128</code> | <code>f64x2\.pmax</code> |  |
| <code>0</code> | <code>func</code> | <code>ref\.null</code> | <code>reftype</code> |
| <code>1</code> | <code>i32</code> | <code>ref\.is\_null</code> |  |
| <code>0</code> | <code>func</code> | <code>ref\.func</code> | <code>funcidx</code> |
| <code>0</code> |  | <code>unreachable</code> |  |
| <code>0</code> |  | <code>nop</code> |  |
| <code>0</code> |  | <code>block</code> | <code>blocktype</code> |
| <code>0</code> |  | <code>loop</code> | <code>blocktype</code> |
| <code>0</code> |  | <code>if</code> | <code>blocktype</code> |
| <code>0</code> |  | <code>else</code> |  |
| <code>0</code> |  | <code>end</code> |  |
| <code>0</code> |  | <code>br</code> | <code>labelidx</code> |
| <code>1</code> |  | <code>br\_if</code> | <code>labelidx</code> |
| <code>1</code> |  | <code>br\_table</code> | <code>labelidx,labelidx,...</code> |
| <code>0</code> |  | <code>return</code> |  |
| <code>0</code> |  | <code>call</code> | <code>funcidx</code> |
| <code>1</code> |  | <code>call\_indirect</code> | <code>tableidx,typeidx</code> |
