
#include "icommand.h"

namespace nut
{

/**
 * 命令名缩写
 *
 * @return 最后一项必须是NULL
 */
const char** ICommand::get_command_nick_names() const
{
    static const char* ret[] = {
        NULL
    };
    return ret;
}

/** 分析命令行 */
std::vector<std::string> ICommand::parse_comand_line(const char *cmd_line)
{
    std::vector<std::string> ret;
    if (NULL == cmd_line)
        return ret;

    // 略过开头的空白
    int last_position = -1;
    while (cmd_line[last_position + 1] != '\0' &&
           (cmd_line[last_position + 1] == ' ' || cmd_line[last_position] == '\t'))
        ++last_position;

    // 拆分各个参数
    int current_position = last_position + 1;
    char quot = '\0';
    while ('\0' != cmd_line[current_position])
    {
        const char c = cmd_line[current_position];
        if ('\0' == quot && ('\"' == c || '\'' == c))
        {
            quot = c;
        }
        else if (c == quot)
        {
            quot = '\0';
        }
        else if ('\0' == quot && ' ' == c)
        {
            if (last_position + 1 < current_position)
                ret.push_back(std::string(cmd_line + (last_position + 1), cmd_line + current_position));
            last_position = current_position;
        }
        ++current_position;
    }
    if (last_position + 1 < current_position)
        ret.push_back(std::string(cmd_line + (last_position + 1), cmd_line + current_position));

    return ret;
}

/** 分析命令行 */
void ICommand::parse_comand_line(const wchar_t *cmd_line, std::vector<std::wstring> *appended)
{
    assert(NULL != appended);
    if (NULL == cmd_line)
        return;

    // 略过开头的空白
    int last_position = -1;
    while (cmd_line[last_position + 1] != L'\0' &&
           (cmd_line[last_position + 1] == L' ' || cmd_line[last_position] == L'\t'))
        ++last_position;

    // 拆分各个参数
    int current_position = last_position + 1;
    wchar_t quot = L'\0';
    while (L'\0' != cmd_line[current_position])
    {
        const wchar_t c = cmd_line[current_position];
        if (L'\0' == quot && (L'\"' == c || L'\'' == c))
        {
            quot = c;
        }
        else if (c == quot)
        {
            quot = L'\0';
        }
        else if (L'\0' == quot && L' ' == c)
        {
            if (last_position + 1 < current_position)
                appended->push_back(std::wstring(cmd_line + (last_position + 1), cmd_line + current_position));
            last_position = current_position;
        }
        ++current_position;
    }
    if (last_position + 1 < current_position)
        appended->push_back(std::wstring(cmd_line + (last_position + 1), cmd_line + current_position));
}

}
