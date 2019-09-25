#ifndef __UITILELAYOUT_H__
#define __UITILELAYOUT_H__

#pragma once

namespace DuiLib {
class DUILIB_API CTileLayoutUI : public CContainerUI
{
public:
    CTileLayoutUI();

    virtual LPCTSTR GetClass() const override;
    virtual LPVOID GetInterface(LPCTSTR pstrName) override;
    virtual void SetPos(RECT rc, bool bNeedInvalidate = true) override;
    virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue) override;

    int GetFixedColumns() const;
    void SetFixedColumns(int iColums);
    int GetFixedRows() const;
    void SetFixedRows(int iRows);
    int GetChildVMargin() const;
    void SetChildVMargin(int iMargin);
    bool GetChildRounded() const;
    void SetChildRounded(bool bRound);

    SIZE GetItemSize() const;
    void SetItemSize(SIZE szSize);
    int GetColumns() const;
    int GetRows() const;

private:
    void ResetInternVisible(void);  // 重置子控件内部显示属性
protected:
    SIZE m_szItem;                  // 属性|当前：子控件大小，默认80
    int m_nColumns;                 // 当前子控件列数
    int m_nRows;                    // 当前子控件行数

    int m_nColumnsFixed;            // 属性：子控件列数
    int m_nRowsFixed;               // 属性：子控件行数
    int m_iChildVMargin;            // 属性：子控件垂直方向间距
    bool m_bChildRounded;           // 属性：子控件水平、垂直铺满，忽略子控件大小
};
}
#endif // __UITILELAYOUT_H__
