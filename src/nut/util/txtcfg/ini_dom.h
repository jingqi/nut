﻿
#ifndef ___HEADFILE_C9DB1FAD_B2DA_45F6_AE36_818B4BB68EC1_
#define ___HEADFILE_C9DB1FAD_B2DA_45F6_AE36_818B4BB68EC1_

#include <vector>
#include <string>

#include <nut/rc/rc_new.h>

#include "../../nut_config.h"
#include "property_dom.h"


namespace nut
{

/**
 * .ini 文件 DOM 结构
 */
class NUT_API IniDom
{
    NUT_REF_COUNTABLE

public:
    IniDom() = default;

    /**
     * @param line_comment_chars 行注释的起始标记字符，可以有多种行注释，如 ';' 行注释和 '#' 行注释
     * @param space_chars 空白字符，其中出现的字符将被视为空白
     */
    void parse(const std::string& s, const char *line_comment_chars = ";#", const char *space_chars = " \t");

    /**
     * @param le 换行符
     */
    std::string serielize(const char *le = "\n") const;

    bool is_dirty() const;

    void set_dirty(bool dirty = true);

    void clear();

    std::vector<std::string> list_sectors() const;
    bool has_sector(const std::string& sector) const;
    bool remove_sector(const std::string& sector);

    std::vector<std::string> list_keys(const std::string& sector) const;
    bool has_key(const std::string& sector, const std::string& key) const;
    bool remove_key(const std::string& sector, const std::string& key);

    const char* get_string(const std::string& sector, const std::string& key,
                           const char *default_value = "") const;
    bool get_bool(const std::string& sector, const std::string& key,
                  bool default_value = false) const;
    long get_int(const std::string& sector, const std::string& key,
                 long default_value = 0) const;
    double get_decimal(const std::string& sector, const std::string& key,
                       double default_value = 0.0) const;
    std::vector<std::string> get_list(const std::string& sector, const std::string& key,
                                      char split_char = ',') const;

    void set_string(const std::string& sector, const std::string& key, const std::string& value);
    void set_bool(const std::string& sector, const std::string& key, bool value);
    void set_int(const std::string& sector, const std::string& key, long value);
    void set_decimal(const std::string& sector, const std::string& key, double value);
    void set_list(const std::string& sector, const std::string& key,
                  const std::vector<std::string>& values, char split_char = ',');

private:
    typedef PropertyDom::Line Line;

    /**
     * 每个块的行是这样构成的
     * space0 '[' space1 name space2 ']' space3 comment
     */
    class Sector
    {
        NUT_REF_COUNTABLE

    public:
        /**
         * @param line_comment_chars 行注释的起始标记字符，可以有多种行注释，如
         *        ';' 行注释和 '#' 行注释
         * @param space_chars 空白字符，其中出现的字符将被视为空白
         */
        static rc_ptr<Sector> parse_sector_name(const std::string& line,
                                                const char *line_comment_chars = ";#",
                                                const char *space_chars = " \t");

        /**
         * @param le 换行符
         */
        std::string serielize(const char *le = "\n");

    public:
        std::string _space0;
        std::string _space1;
        std::string _name;
        std::string _space2;
        std::string _space3;
        std::string _comment;
        std::vector<rc_ptr<Line> > _lines;
    };

    std::vector<rc_ptr<Line>> _anonymous_lines;
    std::vector<rc_ptr<Sector>> _sectors;
    bool _dirty = false;
};

}

#endif
