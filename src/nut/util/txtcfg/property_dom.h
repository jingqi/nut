
#ifndef ___HEADFILE___C3889B7F_89FD_4968_A677_E85C8BB7EC22_
#define ___HEADFILE___C3889B7F_89FD_4968_A677_E85C8BB7EC22_

#include <string>
#include <vector>

#include <nut/rc/rc_new.h>

namespace nut
{

/**
 * .property 文件 DOM 结构
 */
class PropertyDom
{
    NUT_REF_COUNTABLE

    /**
     * 每一行是这样构成的
     * space0 key space1 '=' space2 value space3 comment
     */
    struct Line
    {
        NUT_REF_COUNTABLE

        std::string _space0;
        std::string _key;
        std::string _space1;
        bool _equal_sign = false;
        std::string _space2;
        std::string _value;
        std::string _space3;
        std::string _comment;

        void clear();

        /**
         * @param line 单行字符串，不包含回车换行
         * @param line_comment_chars 行注释的起始标记字符，可以有多种行注释，如 ';' 行注释和 '#' 行注释
         * @param space_chars 空白字符，其中出现的字符将被视为空白
         */
        void parse(const std::string& line, const char *line_comment_chars = ";#", const char *space_chars = " \t");

        /**
         * 序列化，不包含尾部的 '\n'
         */
        void serielize(std::string *appended);
    };
    friend class IniDom;

    std::vector<rc_ptr<Line> > _lines;
    bool _dirty = false;

public:
    PropertyDom();

    /**
     * @param line_comment_chars 行注释的起始标记字符，可以有多种行注释，如 ';' 行注释和 '#' 行注释
     * @param space_chars 空白字符，其中出现的字符将被视为空白
     */
    void parse(const std::string& s, const char *line_comment_chars = ";#", const char *space_chars = " \t");

    /**
     * @param le 换行符
     */
    void serielize(std::string *appended, const char *le = "\n") const;

    bool is_dirty() const;

    void set_dirty(bool dirty = true);

    void clear();

    void list_keys(std::vector<std::string> *rs) const;
    bool has_key(const char *key) const;
    bool remove_key(const char *key);

    const char* get_string(const char *key, const char *default_value = "") const;
    bool get_bool(const char *key, bool default_value = false) const;
    long get_num(const char *key, long default_value = 0) const;
    double get_decimal(const char *key, double default_value = 0.0) const;
    void get_list(const char *key, std::vector<std::string> *rs, char split_char = ',') const;

    void set_string(const char *key, const char *value);
    void set_bool(const char *key, bool value);
    void set_num(const char *key, long value);
    void set_decimal(const char *key, double value);
    void set_list(const char *key, const std::vector<std::string>& values, char split_char = ',');
};

}

#endif /* head file guarder */
