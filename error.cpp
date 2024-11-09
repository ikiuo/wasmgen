/*
 * error.cpp
 */
#include "error.hpp"

namespace wasmgen
{
    ErrorCodeTable error_code_table {
        {ErrorCode::INTERNAL_ERROR, {{0},{"内部エラーが発生しました。"}}},
        {ErrorCode::ASSEMBLY_FAILED, {{0},{"アセンブルに失敗しました。"}}},
        /**/
        {ErrorCode::NO_INCLUDE_FILE, {{0},{"ファイル名がありません。"}}},
        {ErrorCode::INCLUDE_NESTING_LIMIT, {{0},{"多重インクルード制限を超えました。"}}},
        {ErrorCode::NO_FILE, {{0,-1,1},{"ファイル \"","\" が開けません。"}}},
        /**/
        {ErrorCode::EXIST_ALIAS_NAME, {{-1,0},{" は既に定義されています。"}}},
        /**/
        {ErrorCode::NO_ASSEMBLY_SWITCH_CONDITION, {{0},{"アセンブリ条件がありません。"}}},
        {ErrorCode::TOO_MANY_ASSEMBLY_SWITCH_CONDITION, {{0},{"アセンブリ条件が多すぎます。"}}},
        {ErrorCode::NO_ASSEMBLY_SWITCH_IF, {{-1,0},{" に対する @if がありません。"}}},
        {ErrorCode::INVALID_ASSEMBLY_SWITCH_ELIF, {{0},{"@else 後に @elif は配置できません。"}}},
        /**/
        {ErrorCode::NO_MACRO_LABEL, {{-1,0},{" のラベルがありません。"}}},
        {ErrorCode::EXIST_MACRO_NAME, {{0,-1,1},{"マクロ ", " は既に定義されています。"}}},
        {ErrorCode::NESTED_MACRO_DEFINITIONS, {{0},{"マクロ定義が入れ子になっています。"}}},
        {ErrorCode::NO_MACRO_ARGUMENT_NAME, {{0}, {"マクロ定義に引数名がありません。"}}},
        {ErrorCode::LABEL_REPLACEMENT_FAILED, {{0}, {"ラベル名のマクロ展開に失敗しました。"}}},
        {ErrorCode::NESTED_MACRO_EXPANSION, {{0,-1,1},{"マクロ \"","\" 展開が入れ子になっています。"}}},
        {ErrorCode::INVALID_MACRO_NAME, {{0},{"無効なマクロ名です。"}}},
        {ErrorCode::CAN_NOT_DELETE_MACRO, {{0},{"マクロ定義中は削除できません。"}}},
        /**/
        {ErrorCode::NO_STRUCT_LABEL, {{-1,0},{" のラベルがありません。"}}},
        {ErrorCode::EXIST_STRUCT_NAME, {{0,-1,1},{"構造 ", " は既に定義されています。"}}},
        {ErrorCode::EXIST_MEMBER_NAME, {{0,-1,1},{"メンバ名 ", " は既に定義されています。"}}},
        /**/
        {ErrorCode::INVALID_ESCAPE_SEQUENCE, {{0},{"無効なエスケープ シーケンスです。"}}},
        {ErrorCode::INVALID_OCTAL_CODE, {{0},{"無効な文字コードです。"}}},
        {ErrorCode::INVALID_NUMBER_FORMAT, {{0},{"無効な数値表現です。"}}},
        /**/
        {ErrorCode::SYNTAX_ERROR, {{0,-1,1},{"\"", "\" は未知の文法です。"}}},
        {ErrorCode::DIVISION_BY_ZERO, {{0,-1,1},{"ゼロ(", ")除算エラー。"}}},
        {ErrorCode::INFINITE_LOOP, {{0,-1,1},{"無限ループ(", ")を検出しました。"}}},
        {ErrorCode::CAN_NOT_BE_INLINED, {{-1,0},{"はインライン展開できません。"}}},
        /**/
        {ErrorCode::NOT_VALUE, {{-1,0},{" を数値化できません。"}}},
        {ErrorCode::NOT_STRING, {{-1,0},{" を文字列化できません。"}}},
        /**/
        {ErrorCode::UNMATCHED_LIST_SIZES, {{0},{"リストのサイズが一致しません。"}}},
        {ErrorCode::LIST_INDEX_OVERFLOW, {{0},{"リスト要素のインデックスが大きすぎます。"}}},
        {ErrorCode::INVALID_RANGE_STEP, {{0,-1,1},{"範囲指定のステップ値(",")が無効です。"}}},
        {ErrorCode::UNABLE_TO_EXPAND, {{0,-1,1},{"(",")は展開できません。"}}},
        /**/
        {ErrorCode::NO_SECTION_LABEL, {{-1,0},{" のラベルがありません。"}}},
        {ErrorCode::INVALID_SECTION_ID, {{0,-1,1},{"無効なセクション番号(",")です。"}}},
        {ErrorCode::INVALID_SECTION_NAME, {{0,-1,1},{"未知のセクション名(",")です。"}}},
        {ErrorCode::EXIST_CODE_LABEL, {{-1,0},{" は既に存在します。"}}},
        /**/
        {ErrorCode::NO_CUSTOM_SECTION_NAME, {{0},{"カスタムセクションの名前がありません。"}}},
        {ErrorCode::EXIST_CUSTOM_SECTION, {{0},{"カスタムセクションは既に存在します。"}}},
        /**/
        {ErrorCode::NO_TYPE_NAME, {{0,-1,1},{"型名(",")がありません。"}}},
        {ErrorCode::EXIST_TYPE_NAME, {{0,-1,1},{"型名(",")は既に存在します。"}}},
        /**/
        {ErrorCode::NO_MODULE_NAME, {{0,-1,1},{"モジュール名(",")がありません。"}}},
        {ErrorCode::NO_IMPORT_NAME, {{0,-1,1},{"モジュールの識別子(",")がありません。"}}},
        /**/
        {ErrorCode::NO_FUNCTION_NAME, {{0,-1,1},{"関数(",")がありません。"}}},
        {ErrorCode::EXIST_FUNCTION_NAME, {{0,-1,1},{"関数(",")は既に存在します。"}}},
        {ErrorCode::FUNCTION_FIXED_PARAMETERS, {{0,-1,1},{"型指定により(",")は使用できません。"}}},
        /**/
        {ErrorCode::NO_TABLE_NAME, {{0,-1,1},{"テーブル名(",")がありません。"}}},
        {ErrorCode::EXIST_TABLE_NAME, {{0,-1,1},{"テーブル名(",")は既に存在します。"}}},
        /**/
        {ErrorCode::NO_MEMORY_NAME, {{0,-1,1},{"メモリ名(",")がありません。"}}},
        {ErrorCode::EXIST_MEMORY_NAME, {{0,-1,1},{"メモリ名(",")は既に存在します。"}}},
        /**/
        {ErrorCode::EXIST_GLOBAL_NAME, {{0,-1,1},{"グローバル名(",")は既に存在します。"}}},
        {ErrorCode::EXIST_GLOBAL_EXPR, {{0},{"グローバル値は既に指定されています。"}}},
        /**/
        {ErrorCode::NO_EXPORT_SYMBOL, {{0,-1,1},{"エクスポート識別子(",")が見つかりません。"}}},
        {ErrorCode::NO_EXPORT_NAME, {{0},{"エクスポート名がありません。"}}},
        /**/
        {ErrorCode::TOO_MANY_STARTS, {{-1,0},{" セクションは複数指定できません。"}}},
        /**/
        {ErrorCode::EXIST_ELEMENT_NAME, {{0,-1,1},{"エレメント名(",")は既に存在します。"}}},
        {ErrorCode::TOO_MANY_REFTYPE, {{0,-1,1},{"reftype(",")を複数指定できません。"}}},
        {ErrorCode::TOO_MANY_TABLEIDX, {{0,-1,1},{"tableidx(",")を複数指定できません。"}}},
        {ErrorCode::TOO_MANY_OFFSET, {{0},{"複数の offset は指定できません。"}}},
        {ErrorCode::NOT_ELEMENT_MDOE, {{-1,0},{" は Element Section の形式ではありません。"}}},
        {ErrorCode::NOT_ELEMENT_ACTIVE, {{0,-1,1},{"active モードではないため "," を指定できません。"}}},
        {ErrorCode::INVALID_ELEMENT_CODE, {{-1,0},{" を配置できません。値なしの \".offset\" が必要です。"}}},
        {ErrorCode::INVALID_TABLEIDX, {{0,-1,1},{"無効なtableidx(",")です。"}}},
        {ErrorCode::INVALID_OFFSET, {{0,-1,1},{"無効なoffset(",")です。"}}},
        /**/
        {ErrorCode::EXIST_DATA_NAME, {{0,-1,1},{"データ名(",")は既に存在します。"}}},
        {ErrorCode::TOO_MANY_MEMIDX, {{0,-1,1},{"memidx(",")を複数指定できません。"}}},
        {ErrorCode::NOT_DATA_ACTIVE, {{0,-1,1},{"active モードではないため "," を指定できません。"}}},
        {ErrorCode::INVALID_DATA_CODE, {{-1,0},{" を配置できません。"}}},
        /**/
        {ErrorCode::INVALID_INSTRUCTION, {{0,-1,1},{"無効な命令(",")です。"}}},
        {ErrorCode::TOO_FEW_OPERANDS, {{0,-1,1},{"命令(",")のオペランドが足りません。"}}},
        /**/
        {ErrorCode::NOT_REFTYPE, {{0,-1,1},{"(",")は reftype ではありません。"}}},
        {ErrorCode::NOT_VALTYPE, {{0,-1,1},{"(",")は valtype ではありません。"}}},
        {ErrorCode::NO_LIMITS, {{0},{"limits 項目がありません。"}}},
        {ErrorCode::CAN_NOT_EQUAL, {{0},{"代入式は使用できません。"}}},
        /**/
        {ErrorCode::NO_PARAM_LABEL, {{0,-1,1},{"(", ")のラベルがありません。"}}},
        {ErrorCode::EXIST_PARAM_NAME, {{0,-1,1},{"引数名(",")は既に存在します。"}}},
        /**/
        {ErrorCode::OUT_OF_RANGE, {{0,-1,1},{"範囲外(",")です。"}}},
        /*
         *
         */
        {ErrorCode::NESTED_COMMENTS, {{0},{"コメントが入れ子になっています。"}}},
        {ErrorCode::TOO_MANY_OPERANDS, {{0,-2,1,-1,2},{"オペランド","番目(",")以降は無視されます。"}}},
        {ErrorCode::IGNORE_CODE_AFTER_END, {{0,-1,1},{"end より後の命令(",")は無視されます。"}}},
        /**/
        {ErrorCode::IGNORE_UNKNOWN_OPTION, {{0},{"未知のオプションは無視されます。"}}},
        {ErrorCode::IGNORE_OPTION_LABEL, {{0,-1,1},{"オプション擬似命令のラベル(",")は無視されます。"}}},
        /**/
        {ErrorCode::IGNORE_INCLUDE_LABEL, {{0,-1,1},{"ファイル取り込み擬似命令のラベル(",")は無視されます。"}}},
        /**/
        {ErrorCode::TRUNCATED_VALUE, {{0,-1,1},{"値(", ")は切り捨てられます。"}}},
        /**/
        {ErrorCode::NO_END, {{0},{"対応する end がありません。"}}},
        {ErrorCode::UNINTENTIONAL_END, {{0},{"意図した end の種類ではありません。"}}},
        /**/
        {ErrorCode::IGNORE_CODE_LABEL, {{0,-1,1},{"ラベル(",")は無視されます。"}}},
        /**/
        {ErrorCode::IGNORE_MESSAGE_LABEL, {{0,-1,1},{"メッセージ擬似命令のラベル(",")は無視されます。"}}},
        {ErrorCode::IGNORE_CUSTOM_SECTION_LABEL, {{0,-1,1},{"Custom Section のラベル(",")は無視されます。"}}},
        {ErrorCode::IGNORE_EXPORT_SECTION_LABEL, {{0,-1,1},{"Export Section のラベル(",")は無視されます。"}}},
        /**/
        {ErrorCode::IGNORE_FUNCTYPE_PARAMETER, {{0,-1,1},{"関数型表記の3番目(",")以降のパラメータは無視されます。"}}},
        /**/
        {ErrorCode::IGNORE_ASSEMBLE_SWITCH_LABEL, {{0,-1,1},{"アセンブル条件擬似命令のラベル(",")は無視されます。"}}},
        {ErrorCode::ASSEMBLE_SWITCH_NO_ENDIF, {{-1,0},{" に対応する @endif がありません。"}}},
        {ErrorCode::ASSEMBLE_SWITCH_NO_SET, {{0},{"@set 項目がありません"}}},
        {ErrorCode::ASSEMBLE_SWITCH_NO_UNSET, {{0},{"@unset 項目がありません"}}},
        /**/
        {ErrorCode::IGNORE_MACRO_LABEL, {{0,-1,1},{"ラベル(",")は無視されます。"}}},
        {ErrorCode::NO_MACRO_END, {{0},{"マクロ定義の終端がありません。"}}},
        {ErrorCode::NO_MACRO_DEFINITION, {{0}, {"対応する .defmacro がありません。"}}},
        {ErrorCode::MACRO_NOT_DEFINED, {{0,-1,1},{"マクロ名 \"","\" は定義されてません。"}}},
        /**/
        {ErrorCode::NO_STRUCT_DEFINITION, {{0}, {"対応する .defstruct がありません。"}}},
        /**/
        {ErrorCode::TOO_FEW_PARAMETERS, {{0},{"パラメータが足りません。"}}},
        {ErrorCode::PARAMETER_TYPES_DONT_MATCH, {{0},{"パラメータが足りません。"}}},
    };

} // wasmgen
