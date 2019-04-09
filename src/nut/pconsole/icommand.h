
#ifndef ___HEADFILE_D70275E2_739E_4868_AE4A_A5B5E5CB7D38_
#define ___HEADFILE_D70275E2_739E_4868_AE4A_A5B5E5CB7D38_

#include <string>
#include <vector>

#include "../nut_config.h"
#include "../rc/rc_ptr.h"


namespace nut
{

class NUT_API ICommand
{
    NUT_REF_COUNTABLE

public:
    /**
     * 查询命令名
     */
    virtual const char* get_command_name() const = 0;

    /**
     * 命令名缩写
     *
     * @return 最后一项必须是 nullptr
     */
    virtual const char** get_command_nick_names() const;

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

    /**
     * 分析命令行
     */
    static std::vector<std::string> parse_comand_line(const char *cmd_line);

    /**
     * 分析命令行
     */
    static void parse_comand_line(const wchar_t *cmd_line, std::vector<std::wstring> *appended);
};

}

#endif
