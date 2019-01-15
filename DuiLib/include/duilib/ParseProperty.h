#ifndef __PARSEPROPERTY_H__
#define __PARSEPROPERTY_H__
#pragma once

namespace DuiLib {

bool ParseBool(LPCTSTR pstrVal);            // bool         值属性解析

BYTE ParseByte(LPCTSTR pstrVal);            // byte         值属性解析。支持 十六进制 写法 0xE
WORD ParseWord(LPCTSTR pstrVal);            // WORD         值属性解析。支持 十六进制 写法 0XE
INT ParseInt(LPCTSTR pstrVal);              // INT          值属性解析。支持 十六进制 写法 0xE
DWORD ParseDWord(LPCTSTR pstrVal);          // DWORD        值属性解析。支持 十六进制 写法 0XE
DWORD_PTR ParseDWordPtr(LPCTSTR pstrVal);   // DWORD_PTR    值属性解析。支持 十六进制 写法 0xE
float ParseFloat(LPCTSTR pstrVal);          // float        值属性解析

CDuiString ParseString(LPCTSTR pstrVal);    // String/Image 值属性解析。
SIZE ParseSize(LPCTSTR pstrVal);            // SIZE         值属性解析。支持单个 INT 或 INT,INT
RECT ParseRect(LPCTSTR pstrVal);            // RECT         值属性解析。支持单个 INT 或 INT,INT,INT,INT
TPercentInfo ParseRectPercent(LPCTSTR pstrVal); // DoubleRect   值属性解析
DWORD ParseColor(LPCTSTR pstrVal);          // Color        值属性解析。

struct TEffectProperty
{
    BYTE    m_byId;         // 动画特效ID
    bool    m_byDirection;  // 动画播放方向
    bool    m_byLoop;       // 是否循环
    WORD    m_byFrequency;  // 帧持续时间（毫秒）

    TEffectProperty(void) : m_byId(EFFECT_FLIPLEFT), m_byDirection(true), m_byLoop(false), m_byFrequency(150) { }
};

TEffectProperty ParseEffect(LPCTSTR pstrVal);       // 动画特效     值属性解析

}

#endif // __PARSEPROPERTY_H__
