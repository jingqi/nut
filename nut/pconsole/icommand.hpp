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

#include <nut/gc/ref.hpp>

namespace nut
{

class ICommand
{
    NUT_GC_REFERABLE

public:
    /** 查询命令名 */
    virtual const char* getCommandName() const = 0;

    /**
     * 命令名缩写
     * 
     * @return 最后一项必须是NULL
     */
    virtual const char** getCommandNickNames() const
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
    virtual const char* getGeneralInfo() const = 0;

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
    virtual const char* getDetailInfo() const = 0;

    /**
     * 执行命令
     *
     * @param commandLine 命令行，包括起始部分的命令名
     */
    virtual void execute(const char* commandLine) = 0;

public:
    /** 分析命令行 */
    static std::vector<std::string> parseComandLine(const char *cmdLine)
    {
        std::vector<std::string> ret;
        if (NULL == cmdLine)
            return ret;

        // 略过开头的空白
        int lastPosition = -1;
        while (cmdLine[lastPosition + 1] != '\0' &&
               (cmdLine[lastPosition + 1] == ' ' || cmdLine[lastPosition] == '\t'))
            ++lastPosition;

        // 拆分各个参数
        int currentPosition = lastPosition + 1;
        char quot = '\0';
        while ('\0' != cmdLine[currentPosition])
        {
            const char c = cmdLine[currentPosition];
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
                if (lastPosition + 1 < currentPosition)
                    ret.push_back(std::string(cmdLine + (lastPosition + 1), cmdLine + currentPosition));
                lastPosition = currentPosition;
            }
            ++currentPosition;
        }
        if (lastPosition + 1 < currentPosition)
            ret.push_back(std::string(cmdLine + (lastPosition + 1), cmdLine + currentPosition));

        return ret;
    }

    /** 分析命令行 */
    static std::vector<std::wstring> parseComandLine(const wchar_t *cmdLine)
    {
        std::vector<std::wstring> ret;
        if (NULL == cmdLine)
            return ret;

        // 略过开头的空白
        int lastPosition = -1;
        while (cmdLine[lastPosition + 1] != L'\0' &&
               (cmdLine[lastPosition + 1] == L' ' || cmdLine[lastPosition] == L'\t'))
            ++lastPosition;

        // 拆分各个参数
        int currentPosition = lastPosition + 1;
        wchar_t quot = L'\0';
        while (L'\0' != cmdLine[currentPosition])
        {
            const wchar_t c = cmdLine[currentPosition];
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
                if (lastPosition + 1 < currentPosition)
                    ret.push_back(std::wstring(cmdLine + (lastPosition + 1), cmdLine + currentPosition));
                lastPosition = currentPosition;
            }
            ++currentPosition;
        }
        if (lastPosition + 1 < currentPosition)
            ret.push_back(std::wstring(cmdLine + (lastPosition + 1), cmdLine + currentPosition));

        return ret;
    }
};

}

#endif


