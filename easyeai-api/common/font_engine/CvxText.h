/**
 *
 * Copyright 2021 by Guangzhou Easy EAI Technologny Co.,Ltd.
 * website: www.easy-eai.com
 *
 * Author: Jiehao.Zhong <zhongjiehao@easy-eai.com>
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * License file for more details.
 * 
 */

#ifndef CVX_TEXT_H
#define CVX_TEXT_H
 
// OpenCV の中国語漢字入力に対応
 
#include <ft2build.h>
#include FT_FREETYPE_H
 
#include <opencv2/opencv.hpp>
 
class CvxText {
public:
    /**
    * フォントライブラリファイルを読み込みます
    */
    CvxText(const char* freeType);
    virtual ~CvxText();
 
    /**
    * フォントを取得します。現在、一部のパラメータは未対応です。
    *
    * \param font        フォントタイプ。現在は未対応です
    * \param size        フォントサイズ／空白比率／間隔比率／回転角度
    * \param underline   下線
    * \param diaphaneity 透明度
    *
    * \sa setFont, restoreFont
    */
    void getFont(int* type, cv::Scalar* size=nullptr, bool* underline=nullptr, float* diaphaneity=nullptr);
 
    /**
    * フォントを設定します。現在、一部のパラメータは未対応です。
    *
    * \param font        フォントタイプ。現在は未対応です
    * \param size        フォントサイズ／空白比率／間隔比率／回転角度
    * \param underline   下線
    * \param diaphaneity 透明度
    *
    * \sa getFont, restoreFont
    */
    void setFont(int* type, cv::Scalar* size=nullptr, bool* underline=nullptr, float* diaphaneity=nullptr);
 
    /**
    * 元のフォント設定に戻します.
    *
    * \sa getFont, setFont
    */
    void restoreFont();
 
    /**
    * 漢字を出力します（デフォルト色は黒）。出力できない文字に遭遇すると停止します。
    *
    * \param img  出力画像
    * \param text テキスト内容
    * \param pos  テキスト位置
    *
    * \return 成功時は出力文字長を返し、失敗時は -1 を返します。
    */
    int putText(cv::Mat& img, char* text, cv::Point pos);
 
    /**
    * 漢字を出力します（デフォルト色は黒）。出力できない文字に遭遇すると停止します。
    *
    * \param img  出力画像
    * \param text テキスト内容
    * \param pos  テキスト位置
    *
    * \return 成功時は出力文字長を返し、失敗時は -1 を返します。
    */
    int putText(cv::Mat& img, const wchar_t* text, cv::Point pos);
 
    /**
    * 漢字を出力します。出力できない文字に遭遇すると停止します。
    *
    * \param img   出力画像
    * \param text  テキスト内容
    * \param pos   テキスト位置
    * \param color テキスト色
    *
    * \return 成功時は出力文字長を返し、失敗時は -1 を返します。
    */
    int putText(cv::Mat& img, const char* text, cv::Point pos, cv::Scalar color);
 
    /**
    * 漢字を出力します。出力できない文字に遭遇すると停止します。
    *
    * \param img   出力画像
    * \param text  テキスト内容
    * \param pos   テキスト位置
    * \param color テキスト色
    *
    * \return 成功時は出力文字長を返し、失敗時は -1 を返します。
    */
    int putText(cv::Mat& img, const wchar_t* text, cv::Point pos, cv::Scalar color);
 
private:
    // 禁止copy
    CvxText& operator=(const CvxText&);
    // 現在の文字を出力し、m_pos 位置を更新します
    void putWChar(cv::Mat& img, wchar_t wc, cv::Point& pos, cv::Scalar color);
 
    FT_Library   m_library;   // フォントライブラリ
    FT_Face      m_face;      // フォント
 
    // デフォルトのフォント出力パラメータ
    int         m_fontType;
    cv::Scalar  m_fontSize;
    bool        m_fontUnderline;
    float       m_fontDiaphaneity;
};
 
#endif // CVX_TEXT_HPP