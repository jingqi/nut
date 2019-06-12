
#ifndef ___HEADFILE_B694AC90_3B03_460A_A7B6_AAE8EF5A3560_
#define ___HEADFILE_B694AC90_3B03_460A_A7B6_AAE8EF5A3560_

#include <assert.h>
#include <vector>
#include <string>
#include <map>

#include "../../../nut_config.h"
#include "../../../rc/rc_new.h"
#include "xml_writer.h"


namespace nut
{

class NUT_API XmlElement
{
    NUT_REF_COUNTABLE

private:
    typedef std::map<std::string, std::string> attr_map_type;

public:
    typedef attr_map_type::iterator       attr_iter_type;
    typedef attr_map_type::const_iterator const_attr_iter_type;

private:
    class Comment
    {
    public:
        Comment() = default;

        Comment(size_t _pos, const std::string& _text) noexcept
            : pos(_pos), text(_text)
        {}

    public:
        size_t pos = 0;
        std::string text;
    };

public:
    XmlElement() = default;

    explicit XmlElement(const std::string& name) noexcept;

    bool is_dirty() const noexcept;
    void set_dirty(bool dirty) noexcept;

    const std::string& get_name() const noexcept;
    void set_name(const std::string& name) noexcept;

    const std::string& get_text() const noexcept;
    void set_text(const std::string& text) noexcept;

    size_t get_children_count() const noexcept;

    rc_ptr<XmlElement> get_child(size_t i) const noexcept;
    rc_ptr<XmlElement> get_child(const std::string& name) const noexcept;

    void append_child(rc_ptr<XmlElement> child) noexcept;
    void insert_child(size_t pos, rc_ptr<XmlElement> child) noexcept;
    void remove_child(size_t pos) noexcept;

    void clear_children() noexcept;

    /**
     * 获取属性
     *
     * @return 如果属性不存在，会返回 nullptr
     */
    const std::string* get_attribute(const std::string& name) const noexcept;

    /**
     * 添加属性
     *
     * @return 是否成功，如果属性已经存在，则添加失败
     */
    bool add_attribute(const std::string& name, const std::string& value) noexcept;

    /**
     * 存在属性则设置属性值，否则添加新属性
     */
    void set_attribute(const std::string& name, const std::string& value) noexcept;

    bool remove_attribute(const std::string& name) noexcept;

    void clear_attributes() noexcept;

    void add_comment(size_t pos, const std::string& text) noexcept;
    void remove_comment(size_t pos) noexcept;

    void clear() noexcept;

    const_attr_iter_type attr_const_begin() const noexcept;
    const_attr_iter_type attr_const_end() const noexcept;

    attr_iter_type attr_begin() noexcept;
    attr_iter_type attr_end() noexcept;

    void parse(const std::string& s, size_t start_index = 0, bool ignore_text_blank = true) noexcept;

    /**
     * @param format 格式化输出，以便于阅读
     */
    std::string serielize(bool format = true) const noexcept;

    void serielize(XmlWriter &writer, int tab) const noexcept;

private:
    std::string _name, _text;
    attr_map_type _attrs;
    std::vector<rc_ptr<XmlElement> > _children;
    std::vector<Comment> _comments; // sorted ascending
    bool _dirty = false;
};

}

#endif
