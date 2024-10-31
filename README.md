# wasmgen - WebAssembly 用アセンブラ

[WebAssembly](https://webassembly.github.io/spec/core/)の[バイナリ形式(.wasm)](https://webassembly.github.io/spec/core/binary/)を生成するための1行1命令形式アセンブラです。対応しているファイルの文字コードは UTF-8/16/32(+BOM) です。他のアセンブラとの互換性はなく、[テキスト形式(.wat)](https://webassembly.github.io/spec/core/text/)よりも冗長になります。数値のオーバーフローや .wasm 適合などのチェック処理は行っていませんので、記述には十分注意してください。生成された .wasm 内容の詳細を確認するために[wasmdump](https://github.com/ikiuo/WebAssembly-wasmdump)を用意しています。


&nbsp;

# wasmgen のビルド

ソースコードは C++14 を前提として記述しています。


### Unix 系

[GNU make](https://www.gnu.org/software/make/) を使ってビルドします。コンパイラは [clang++](https://clang.llvm.org/) を使用しています。

GNUmakefile.config で [zlib](https://zlib.net/) ライブラリの使用を指定すると、.wasm を gzip/defalte 圧縮形式にして出力できます。

```
# GNUmakefile.config
CXXDEFS = -DUSE_ZLIB
LIBS    = -lz
```


### Windows

VisualStudio (2022) では wasmgen.vcxproj を使用してください。zlib 対応の設定はしていません。


&nbsp;


# ドキュメント

ドキュメントは <code>doc/html</code> にあります。
