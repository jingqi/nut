/**
 * @file -
 * @author jingqi
 * @date 2012-12-10
 * @last-edit 2012-12-10 21:18:39 jingqi
 * @brief
 */

#ifndef ___HEADFILE_D70275E2_739E_4868_AE4A_A5B5E5CB7D38_
#define ___HEADFILE_D70275E2_739E_4868_AE4A_A5B5E5CB7D38_

#include <string>
#include <vector>

#include <nut/rc/rc_ptr.hpp>

namespace nut
{

class ICommand
{
    NUT_REF_COUNTABLE

public:
    /** 查询命令名 */
    virtual const char* get_command_name() const = 0;

    /**
     * 命令名缩写
     * 
     * @return 最后一项必须是NULL
     */
    virtual const char** get_command_nick_names() const
    {
        static const char* ret[] = {
            NULL
        };
        return ret;
    }

    /**
     * 查询命令的简要描述
     * 
     * @return 简要描述，例如
     *     "这是一个测试命令"
     */
    virtual const char* get_general_info() const = 0;

    /**
     * 查询命令的详细描述
     * 
     * @return 详细描述，例如
     *    "testCommand Msg\n
     *     ep.:\n
     *     \tMsg, 消息\n
     *     eg.:\n
     *     \ttestCommand 这是第一个消息"
     */
    virtual const char* get_detail_info() const = 0;

    /**
     * 执行命令
     *
     * @param command_line 命令行，包括起始部分的命令名
     */
    virtual void execute(const char* command_line) = 0;

public:
    /** 分析命令行 */
    static std::vector<std::string> parse_comand_line(const char *cmd_line)
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
    static void parse_comand_line(const wchar_t *cmd_line, std::vector<std::wstring> *appended)
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
};

}

#endif


