
#include <iostream>

#include <nut/unittest/unittest.h>
#include <nut/security/encrypt/pem/pem.h>


using namespace std;
using namespace nut;

class TestPem : public TestFixture
{
    virtual void register_cases() override
    {
        NUT_REGISTER_CASE(test_read_pkcs1_public);
        NUT_REGISTER_CASE(test_read_pkcs1_private);
        NUT_REGISTER_CASE(test_read_pkcs8_public);
        NUT_REGISTER_CASE(test_read_pkcs8_private);
        NUT_REGISTER_CASE(test_write_pkcs1_public);
        NUT_REGISTER_CASE(test_write_pkcs1_private);
        NUT_REGISTER_CASE(test_write_pkcs8_public);
        NUT_REGISTER_CASE(test_write_pkcs8_private);
    }

    void test_read_pkcs1_public()
    {
        const char *pem =
            "-----BEGIN RSA PUBLIC KEY-----"
            "MIIBCgKCAQEA61BjmfXGEvWmegnBGSuS+rU9soUg2FnODva32D1AqhwdziwHINFa"
            "D1MVlcrYG6XRKfkcxnaXGfFDWHLEvNBSEVCgJjtHAGZIm5GL/KA86KDp/CwDFMSw"
            "luowcXwDwoyinmeOY9eKyh6aY72xJh7noLBBq1N0bWi1e2i+83txOCg4yV2oVXhB"
            "o8pYEJ8LT3el6Smxol3C1oFMVdwPgc0vTl25XucMcG/ALE/KNY6pqC2AQ6R2ERlV"
            "gPiUWOPatVkt7+Bs3h5Ramxh7XjBOXeulmCpGSynXNcpZ/06+vofGi/2MlpQZNhH"
            "Ao8eayMp6FcvNucIpUndo1X8dKMv3Y26ZQIDAQAB"
            "-----END RSA PUBLIC KEY-----";
        RSA::PublicKey key;
        NUT_TA(pem_read_rsa_public(pem, &key));

        // cout << key.n.to_string(16) << endl;
        NUT_TA(key.n.to_string(16) ==
               "EB506399F5C612F5A67A09C1192B92FAB53DB28520D859CE0EF6B7D83D40AA1C"
               "1DCE2C0720D15A0F531595CAD81BA5D129F91CC6769719F1435872C4BCD05211"
               "50A0263B470066489B918BFCA03CE8A0E9FC2C0314C4B096EA30717C03C28CA2"
               "9E678E63D78ACA1E9A63BDB1261EE7A0B041AB53746D68B57B68BEF37B713828"
               "38C95DA8557841A3CA58109F0B4F77A5E929B1A25DC2D6814C55DC0F81CD2F4E"
               "5DB95EE70C706FC02C4FCA358EA9A82D8043A47611195580F89458E3DAB5592D"
               "EFE06CDE1E516A6C61ED78C13977AE9660A9192CA75CD72967FD3AFAFA1F1A2F"
               "F6325A5064D847028F1E6B2329E8572F36E708A549DDA355FC74A32FDD8DBA65");
        NUT_TA(key.e == 65537);
    }

    void test_read_pkcs1_private()
    {
        const char *pem =
            "-----BEGIN RSA PRIVATE KEY-----"
            "MD8CAQACCQC8LGbl5kjo7wIDAQABAggEoZ8irJt1iQIFANxhrRMCBQDalhw1AgRk"
            "5rvNAgUAydplIQIFAMVXS/0="
            "-----END RSA PRIVATE KEY-----";
        RSA::PrivateKey key;
        NUT_TA(pem_read_rsa_private(pem, &key));

        NUT_TA(key.n.to_string(16) == "BC2C66E5E648E8EF");
        NUT_TA(key.e == 65537);
        NUT_TA("0" + key.d.to_string(16) == "04A19F22AC9B7589");
        NUT_TA(key.p.to_string(16) == "DC61AD13");
        NUT_TA(key.q.to_string(16) == "DA961C35");
    }

    void test_read_pkcs8_public()
    {
        const char *pem =
            "-----BEGIN PUBLIC KEY-----"
            "MCQwDQYJKoZIhvcNAQEBBQADEwAwEAIJALwsZuXmSOjvAgMBAAE="
            "-----END PUBLIC KEY-----";
        RSA::PublicKey key;
        NUT_TA(pem_read_rsa_public(pem, &key));

        // cout << key.n.to_string(16) << endl;
        NUT_TA(key.n.to_string(16) == "BC2C66E5E648E8EF");
        NUT_TA(key.e == 65537);
    }

    void test_read_pkcs8_private()
    {
        const char *pem =
            "-----BEGIN PRIVATE KEY-----"
            "MIICdwIBADANBgkqhkiG9w0BAQEFAASCAmEwggJdAgEAAoGBANESTe7LjH2LhrXo"
            "5g5gSnkivZ/XqWyZQcHYeMYOXGRTSO71gCnJ5mVRdvX3VmTEna/Hb68qmk3iAosP"
            "LmvskxOnByHUI29x7JJfoOIziXBMCdQRmIFiA0E2sog0S0mZdZJkFN5Hu/scf8TE"
            "0/m/KGGTTovWU6iSeFhyr30WNMaHAgMBAAECgYAE609WHQfzNEM5KH+xOubFruGT"
            "Tzm3SmvXqcY9srzNx3/hz3jygsOfAqmv49/ugwnKxwCDuJbk2jqBFxxagbh8JPmM"
            "G8nNwyWhmcDfqwTRu3OVOP//vXigFiIRw7hpYLskHIyBVZnZZqSotAaiK/igp3OL"
            "OBzQuYILdczn4X5GkQJBAPjIdAm4l3r+OBcgTy/zcCJQkOHQ3yc63ZIapbNmlgoz"
            "U5RAhX6yzdKRHPnJap9QEqDJTw5WRFaPvxG5mLZayKMCQQDXIvCj6muQGbmuBtAg"
            "ZGcMnkFt//T+n59Il+ba2JDWj3hOO6Emzj16EUu0kqlBP3fGU6cXAjusAlZVR0s9"
            "SzTNAkAvjqIf+Zl7eX1fbl203ORiquQHRtZhuW8BrvZeBQ5JhOZFQNBEGAogZn0T"
            "gt1O9w+YjOL/6p3FrlToHoKC2XfhAkEAipTPPkd7Ek//88Ifvz3tw4sNyrXeM0bP"
            "bAutgbuPUScJ8BspK74ei8soYSE4NfeUSAUK1R9zINJAmp5aMRmI4QJBAL29qAbT"
            "53Eua+VeDqxgQ3Vz54jwokhqkqDRcHM8Cphx9PZyEXd1Q8DehN+uTnUxsdoaMjVS"
            "R3vpWnZDzckKVuk="
            "-----END PRIVATE KEY-----";
        RSA::PrivateKey key;
        NUT_TA(pem_read_rsa_private(pem, &key));

        NUT_TA(key.n.to_string(16) ==
               "D1124DEECB8C7D8B86B5E8E60E604A7922BD9FD7A96C9941C1D878C60E5C6453"
               "48EEF58029C9E6655176F5F75664C49DAFC76FAF2A9A4DE2028B0F2E6BEC9313"
               "A70721D4236F71EC925FA0E23389704C09D411988162034136B288344B499975"
               "926414DE47BBFB1C7FC4C4D3F9BF2861934E8BD653A892785872AF7D1634C687");
        NUT_TA(key.e == 65537);
        NUT_TA("0" + key.d.to_string(16) ==
               "04EB4F561D07F3344339287FB13AE6C5AEE1934F39B74A6BD7A9C63DB2BCCDC7"
               "7FE1CF78F282C39F02A9AFE3DFEE8309CAC70083B896E4DA3A81171C5A81B87C"
               "24F98C1BC9CDC325A199C0DFAB04D1BB739538FFFFBD78A0162211C3B86960BB"
               "241C8C815599D966A4A8B406A22BF8A0A7738B381CD0B9820B75CCE7E17E4691");
        NUT_TA(key.p.to_string(16) ==
               "F8C87409B8977AFE3817204F2FF370225090E1D0DF273ADD921AA5B366960A33"
               "539440857EB2CDD2911CF9C96A9F5012A0C94F0E5644568FBF11B998B65AC8A3");
        NUT_TA(key.q.to_string(16) ==
               "D722F0A3EA6B9019B9AE06D02064670C9E416DFFF4FE9F9F4897E6DAD890D68F"
               "784E3BA126CE3D7A114BB492A9413F77C653A717023BAC025655474B3D4B34CD");
    }

    void test_write_pkcs1_public()
    {
        const char *pem1 =
            "-----BEGIN RSA PUBLIC KEY-----" "\n"
            "MIIBCgKCAQEA61BjmfXGEvWmegnBGSuS+rU9soUg2FnODva32D1AqhwdziwHINFa" "\n"
            "D1MVlcrYG6XRKfkcxnaXGfFDWHLEvNBSEVCgJjtHAGZIm5GL/KA86KDp/CwDFMSw" "\n"
            "luowcXwDwoyinmeOY9eKyh6aY72xJh7noLBBq1N0bWi1e2i+83txOCg4yV2oVXhB" "\n"
            "o8pYEJ8LT3el6Smxol3C1oFMVdwPgc0vTl25XucMcG/ALE/KNY6pqC2AQ6R2ERlV" "\n"
            "gPiUWOPatVkt7+Bs3h5Ramxh7XjBOXeulmCpGSynXNcpZ/06+vofGi/2MlpQZNhH" "\n"
            "Ao8eayMp6FcvNucIpUndo1X8dKMv3Y26ZQIDAQAB" "\n"
            "-----END RSA PUBLIC KEY-----";
        RSA::PublicKey key;
        NUT_TA(pem_read_rsa_public(pem1, &key));

        const string pem2 = pem_write_rsa_public(key);
        // cout << endl << pem1 << endl << pem2 << endl;
        NUT_TA(pem2 == pem1);
    }

    void test_write_pkcs1_private()
    {
        const char *pem1 =
            "-----BEGIN RSA PRIVATE KEY-----" "\n"
            "MD8CAQACCQC8LGbl5kjo7wIDAQABAggEoZ8irJt1iQIFANxhrRMCBQDalhw1AgRk" "\n"
            "5rvNAgUAydplIQIFAMVXS/0=" "\n"
            "-----END RSA PRIVATE KEY-----";
        RSA::PrivateKey key;
        NUT_TA(pem_read_rsa_private(pem1, &key));

        const string pem2 = pem_write_rsa_private(key);
        // cout << endl << pem1 << endl << pem2 << endl;
        NUT_TA(pem2 == pem1);
    }

    void test_write_pkcs8_public()
    {
        const char *pem1 =
            "-----BEGIN PUBLIC KEY-----" "\n"
            "MCQwDQYJKoZIhvcNAQEBBQADEwAwEAIJALwsZuXmSOjvAgMBAAE=" "\n"
            "-----END PUBLIC KEY-----";
        RSA::PublicKey key;
        NUT_TA(pem_read_rsa_public(pem1, &key));

        const string pem2 = pem_write_rsa_public(key, true);
        // cout << endl << pem1 << endl << pem2 << endl;
        NUT_TA(pem2 == pem1);
    }

    void test_write_pkcs8_private()
    {
        const char *pem1 =
            "-----BEGIN PRIVATE KEY-----" "\n"
            "MIICdwIBADANBgkqhkiG9w0BAQEFAASCAmEwggJdAgEAAoGBANESTe7LjH2LhrXo" "\n"
            "5g5gSnkivZ/XqWyZQcHYeMYOXGRTSO71gCnJ5mVRdvX3VmTEna/Hb68qmk3iAosP" "\n"
            "LmvskxOnByHUI29x7JJfoOIziXBMCdQRmIFiA0E2sog0S0mZdZJkFN5Hu/scf8TE" "\n"
            "0/m/KGGTTovWU6iSeFhyr30WNMaHAgMBAAECgYAE609WHQfzNEM5KH+xOubFruGT" "\n"
            "Tzm3SmvXqcY9srzNx3/hz3jygsOfAqmv49/ugwnKxwCDuJbk2jqBFxxagbh8JPmM" "\n"
            "G8nNwyWhmcDfqwTRu3OVOP//vXigFiIRw7hpYLskHIyBVZnZZqSotAaiK/igp3OL" "\n"
            "OBzQuYILdczn4X5GkQJBAPjIdAm4l3r+OBcgTy/zcCJQkOHQ3yc63ZIapbNmlgoz" "\n"
            "U5RAhX6yzdKRHPnJap9QEqDJTw5WRFaPvxG5mLZayKMCQQDXIvCj6muQGbmuBtAg" "\n"
            "ZGcMnkFt//T+n59Il+ba2JDWj3hOO6Emzj16EUu0kqlBP3fGU6cXAjusAlZVR0s9" "\n"
            "SzTNAkAvjqIf+Zl7eX1fbl203ORiquQHRtZhuW8BrvZeBQ5JhOZFQNBEGAogZn0T" "\n"
            "gt1O9w+YjOL/6p3FrlToHoKC2XfhAkEAipTPPkd7Ek//88Ifvz3tw4sNyrXeM0bP" "\n"
            "bAutgbuPUScJ8BspK74ei8soYSE4NfeUSAUK1R9zINJAmp5aMRmI4QJBAL29qAbT" "\n"
            "53Eua+VeDqxgQ3Vz54jwokhqkqDRcHM8Cphx9PZyEXd1Q8DehN+uTnUxsdoaMjVS" "\n"
            "R3vpWnZDzckKVuk=" "\n"
            "-----END PRIVATE KEY-----";
        RSA::PrivateKey key;
        NUT_TA(pem_read_rsa_private(pem1, &key));

        const string pem2 = pem_write_rsa_private(key, true);
        // cout << endl << pem1 << endl << pem2 << endl;
        NUT_TA(pem2 == pem1);
    }
};

NUT_REGISTER_FIXTURE(TestPem, "security, encrypt, quiet")
