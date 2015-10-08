
#ifndef ___HEADFILE_C9DB1FAD_B2DA_45F6_AE36_818B4BB68EC1_
#define ___HEADFILE_C9DB1FAD_B2DA_45F6_AE36_818B4BB68EC1_

#include <vector>
#include <string>

#include <nut/rc/rc_new.h>

#include "property_dom.h"

namespace nut
{

/**
 * .ini 文件 DOM 结构
 */
class IniDom
{
    NUT_REF_COUNTABLE

    typedef PropertyDom::Line Line;

    /**
     * 每个块的行是这样构成的
     * space0 '[' space1 name space2 ']' space3 comment
     */
    struct Sector
    {
        NUT_REF_COUNTABLE

        std::string _space0;
        std::string _space1;
        std::string _name;
        std::string _space2;
        std::string _space3;
        std::string _comment;
        std::vector<rc_ptr<Line> > _lines;

		/**
		 * @param line_comment_chars 行注释的起始标记字符，可以有多种行注释，如 ';' 行注释和 '#' 行注释
		 * @param space_chars 空白字符，其中出现的字符将被视为空白
		 */
        static rc_ptr<Sector> parse_sector_name(const std::string& line, const char *line_comment_chars = ";#", const char *space_chars = " \t");

		/**
		 * @param le 换行符
		 */
        void serielize(std::string *appended, const char *le = "\n");
    };

    std::vector<rc_ptr<Line> > _global_lines;
    std::vector<rc_ptr<Sector> > _sectors;
    bool _dirty = false;

public:
    IniDom();

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

    void list_sectors(std::vector<std::string> *rs) const;
    bool has_sector(const char *sector) const;
    bool remove_sector(const char *sector);

    void list_keys(const char *sector, std::vector<std::string> *rs) const;
    bool has_key(const char *sector, const char *key) const;
    bool remove_key(const char *sector, const char *key);

    const char* get_string(const char *sector, const char *key, const char *default_value = "") const;
    bool get_bool(const char *sector, const char *key, bool default_value = false) const;
    long get_num(const char *sector, const char *key, long default_value = 0) const;
    double get_decimal(const char *sector, const char *key, double default_value = 0.0) const;
    void get_list(const char *sector, const char *key, std::vector<std::string> *rs, char split_char = ',') const;

    void set_string(const char *sector, const char *key, const char *value);
    void set_bool(const char *sector, const char *key, bool value);
    void set_num(const char *sector, const char *key, long value);
    void set_decimal(const char *sector, const char *key, double value);
    void set_list(const char *sector, const char *key, const std::vector<std::string>& values, char split_char = ',');
};

}

#endif
