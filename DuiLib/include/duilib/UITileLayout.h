#ifndef __UITILELAYOUT_H__
#define __UITILELAYOUT_H__

#pragma once

namespace DuiLib {
class DUILIB_API CTileLayoutUI : public CContainerUI
{
public:
    CTileLayoutUI();

    LPCTSTR GetClass() const;
    LPVOID GetInterface(LPCTSTR pstrName);

    void SetPos(RECT rc, bool bNeedInvalidate = true);

    int GetFixedColumns() const;
    void SetFixedColumns(int iColums);
    int GetFixedRows() const;
    void SetFixedRows(int iRows);
    int GetChildVPadding() const;
    void SetChildVPadding(int iPadding);
    bool GetChildRounded() const;
    void SetChildRounded(bool bRound);

    SIZE GetItemSize() const;
    void SetItemSize(SIZE szSize);
    int GetColumns() const;
    int GetRows() const;
    void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

private:
    void ResetInternVisible(void);  // 重置子控件内部显示属性
protected:
    SIZE m_szItem;                  // 属性|当前：子控件大小，默认80
    int m_nColumns;                 // 当前子控件列数
    int m_nRows;                    // 当前子控件行数

    int m_nColumnsFixed;            // 属性：子控件列数
    int m_nRowsFixed;               // 属性：子控件行数
    int m_iChildVPadding;           // 属性：子控件垂直方向间距
    bool m_bChildRounded;           // 属性：子控件水平、垂直铺满，忽略子控件大小
};
}
#endif // __UITILELAYOUT_H__
