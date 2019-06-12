
#ifndef ___HEADFILE___C3889B7F_89FD_4968_A677_E85C8BB7EC22_
#define ___HEADFILE___C3889B7F_89FD_4968_A677_E85C8BB7EC22_

#include <string>
#include <vector>

#include "../../nut_config.h"
#include "../../rc/rc_new.h"


namespace nut
{

/**
 * .property 文件 DOM 结构
 */
class NUT_API PropertyDom
{
    NUT_REF_COUNTABLE

public:
    PropertyDom() = default;

    /**
     * @param line_comment_chars 行注释的起始标记字符，可以有多种行注释，如 ';' 行注释和 '#' 行注释
     * @param space_chars 空白字符，其中出现的字符将被视为空白
     */
    void parse(const std::string& s, const char *line_comment_chars = ";#",
               const char *space_chars = " \t") noexcept;

    /**
     * @param le 换行符
     */
    std::string serielize(const char *le = "\n") const noexcept;

    bool is_dirty() const noexcept;

    void set_dirty(bool dirty = true) noexcept;

    void clear() noexcept;

    std::vector<std::string> list_keys() const noexcept;
    bool has_key(const std::string& key) const noexcept;
    bool remove_key(const std::string& key) noexcept;

    const char* get_string(const std::string& key, const char *default_value = "") const noexcept;
    bool get_bool(const std::string& key, bool default_value = false) const noexcept;
    long get_int(const std::string& key, long default_value = 0) const noexcept;
    double get_decimal(const std::string& key, double default_value = 0.0) const noexcept;
    std::vector<std::string> get_list(const std::string& key, char split_char = ',') const noexcept;

    void set_string(const std::string& key, const std::string& value) noexcept;
    void set_bool(const std::string& key, bool value) noexcept;
    void set_int(const std::string& key, long value) noexcept;
    void set_decimal(const std::string& key, double value) noexcept;
    void set_list(const std::string& key, const std::vector<std::string>& values,
                  char split_char = ',') noexcept;

private:
    /**
     * 每一行是这样构成的
     * space0 key space1 '=' space2 value space3 comment
     */
    class Line
    {
        NUT_REF_COUNTABLE

    public:
        void clear() noexcept;

        /**
         * @param line 单行字符串，不包含回车换行
         * @param line_comment_chars 行注释的起始标记字符，可以有多种行注释，如 ';' 行注释和 '#' 行注释
         * @param space_chars 空白字符，其中出现的字符将被视为空白
         */
        void parse(const std::string& line, const char *line_comment_chars = ";#",
                   const char *space_chars = " \t") noexcept;

        /**
         * 序列化，不包含尾部的 '\n'
         */
        std::string serielize() noexcept;

    public:
        std::string _space0;
        std::string _key;
        std::string _space1;
        bool _equal_sign = false;
        std::string _space2;
        std::string _value;
        std::string _space3;
        std::string _comment;
    };
    friend class IniDom;

    std::vector<rc_ptr<Line> > _lines;
    bool _dirty = false;
};

}

#endif /* head file guarder */
