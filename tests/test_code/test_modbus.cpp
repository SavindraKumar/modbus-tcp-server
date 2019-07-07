#include "CppUTest/TestHarness.h"
#include <string.h>
#include <stdio.h>


extern "C"
{
    #include "mbap_conf.h"
    #include "mbap.h"
    #include "mbap_user.h"
}

#define QUERY_SIZE_IN_BYTES              (255u)
#define RESPONSE_SIZE_IN_BYTES           (255u)
#define MBT_EXCEPTION_PACKET_LEN         (9u)
#define QUERY_LEN                        (12u)
#define NO_OF_DATA_OFFSET                (10u)
#define DATA_START_ADDRESS_OFFSET        (8u)
#define REGISTER_VALUE_OFFSET            (10u)
//PDU Offset in response
#define MBT_BYTE_COUNT_OFFSET            (8u)
#define MBT_DATA_VALUES_OFFSET           (9u)
#define MBAP_HEADER_LEN                  (7u)



TEST_GROUP(Module)
{
    uint8_t      *pucQuery           = NULL;
    uint8_t      *pucResponse        = NULL;

    void setup()
    {
        pucQuery     = (uint8_t*)calloc(QUERY_SIZE_IN_BYTES,  sizeof(uint8_t));
        pucResponse  = (uint8_t*)calloc(RESPONSE_SIZE_IN_BYTES, sizeof(uint8_t));
        //Init modbus data
        mu_Init();
    }

    void teardown()
    {
        free(pucQuery);
        free(pucResponse);
    }
};

TEST(Module, WrongPorotocolIdTest)
{
    uint8_t ucQueryBuf[QUERY_LEN] = {0, 0, 0, 0, 0, 6, 2, 4, 0, 5, 0, 3};
    
    memcpy(pucQuery, ucQueryBuf, QUERY_LEN);
    //function under test
    uint8_t ucResponseLen = mbap_ProcessRequest(pucQuery, QUERY_LEN, pucResponse);
    //check function return value
    CHECK_EQUAL(0, ucResponseLen);
}

TEST(Module, WrongDeviceIdTest)
{
    uint8_t ucQueryBuf[QUERY_LEN] = {0, 0, 0, 1, 0, 6, 1, 4, 0, 5, 0, 3};
    
    memcpy(pucQuery, ucQueryBuf, QUERY_LEN);
    //function under test
    uint8_t ucResponseLen = mbap_ProcessRequest(pucQuery, QUERY_LEN, pucResponse);
    //check function return value
    CHECK_EQUAL(0, ucResponseLen);
}

TEST(Module, PduLengthExceedTest)
{
    uint8_t ucQueryBuf[QUERY_LEN] = {0, 0, 0, 1, 6, 6, 1, 4, 0, 5, 0, 3};
    
    memcpy(pucQuery, ucQueryBuf, QUERY_LEN);
    //function under test
    uint8_t ucResponseLen = mbap_ProcessRequest(pucQuery, QUERY_LEN, pucResponse);
    //check function return value
    CHECK_EQUAL(0, ucResponseLen);
}

TEST(Module, ReadInputRegistersTest)
{
    uint8_t  ucQueryBuf[QUERY_LEN] = {0, 0, 0, 0, 0, 6, 1, 4, 0, 5, 0, 3};
    uint8_t  ucByteCount           = 0;
    uint16_t usNumOfData           = 0;
    uint16_t usStartAddress        = 0;
    
    //set up test
    usStartAddress  = (uint16_t)(ucQueryBuf[DATA_START_ADDRESS_OFFSET] << 8);
    usStartAddress |= (uint16_t)(ucQueryBuf[DATA_START_ADDRESS_OFFSET + 1]);
    usNumOfData     = (uint16_t)(ucQueryBuf[NO_OF_DATA_OFFSET] << 8);
    usNumOfData    |= (uint16_t)(ucQueryBuf[NO_OF_DATA_OFFSET + 1]);
    ucByteCount     = usNumOfData * 2;
    
    memcpy(pucQuery, ucQueryBuf, QUERY_LEN);
    
    //expected response length
    uint8_t usExpectedResponseLen = MBAP_HEADER_LEN  + (usNumOfData * 2) + 2;
    //function under test
    uint8_t usRecResponseLen      = mbap_ProcessRequest(pucQuery, QUERY_LEN, pucResponse);   
    //check return value from test function
    CHECK_EQUAL(usExpectedResponseLen, usRecResponseLen);
    //check byte count received in response
    CHECK_EQUAL(ucByteCount, pucResponse[MBT_BYTE_COUNT_OFFSET]);
    
    //check values of input registers received with input register data 
    for (uint8_t ucCount = 0; ucCount < usNumOfData; ucCount++)
    {
        int16_t sReceivedValue = 0;
        
        sReceivedValue  = (int16_t)(pucResponse[MBT_DATA_VALUES_OFFSET + (ucCount * 2)] << 8);
        sReceivedValue |= (int16_t)(pucResponse[MBT_DATA_VALUES_OFFSET + (ucCount * 2) + 1]);
        
        CHECK_EQUAL(g_sInputRegsBuf[ucCount + usStartAddress], sReceivedValue);	
    }
}

TEST(Module, IllegalAddressInReadInputRegistersTest)
{
    uint8_t ucQueryBuf[QUERY_LEN] = {0, 0, 0, 0, 0, 6, 1, 4, 0, 0, 0, 16};
    
    memcpy(pucQuery, ucQueryBuf, QUERY_LEN);
  
    //function under test
    uint8_t usRecResponseLen      = mbap_ProcessRequest(pucQuery, QUERY_LEN, pucResponse);
    //expected response length
    uint8_t usExpectedResponseLen = MBT_EXCEPTION_PACKET_LEN;
    //check return value from test function
    CHECK_EQUAL(usExpectedResponseLen, usRecResponseLen);
    //check type of error from response
    CHECK_EQUAL(eILLEGAL_DATA_ADDRESS, pucResponse[MBT_BYTE_COUNT_OFFSET] );	
}

TEST(Module, ReadHoldingRegistersTest)
{
    uint8_t  ucQueryBuf[QUERY_LEN] = {0, 0, 0, 0, 0, 6, 1, 3, 0, 5, 0, 3};
    uint8_t  ucByteCount           = 0;
    uint16_t usNumOfData           = 0;
    uint16_t usStartAddress        = 0;

    //set up test
    usStartAddress  = (uint16_t)(ucQueryBuf[DATA_START_ADDRESS_OFFSET] << 8);
    usStartAddress |= (uint16_t)(ucQueryBuf[DATA_START_ADDRESS_OFFSET + 1]);
    usNumOfData     = (uint16_t)(ucQueryBuf[NO_OF_DATA_OFFSET] << 8);
    usNumOfData    |= (uint16_t)(ucQueryBuf[NO_OF_DATA_OFFSET + 1]);
    ucByteCount     = usNumOfData * 2;
    
    memcpy(pucQuery, ucQueryBuf, QUERY_LEN);
    
    //function under test
    uint8_t usRecResponseLen      = mbap_ProcessRequest(pucQuery, QUERY_LEN, pucResponse);
    //expected response length
    uint8_t usExpectedResponseLen = MBAP_HEADER_LEN  + (usNumOfData * 2) + 2;
    //check return value from test function
    CHECK_EQUAL(usExpectedResponseLen, usRecResponseLen);
    //check byte count from response
    CHECK_EQUAL(ucByteCount, pucResponse[MBT_BYTE_COUNT_OFFSET]);
    
    //check holding registers values from user data
    for (uint8_t ucCount = 0; ucCount < usNumOfData; ucCount++)
    {
        int16_t sReceivedValue = 0;
        
        sReceivedValue  = (int16_t)(pucResponse[MBT_DATA_VALUES_OFFSET + (ucCount * 2)] << 8);
        sReceivedValue |= (int16_t)(pucResponse[MBT_DATA_VALUES_OFFSET + (ucCount * 2) + 1]);
        
        CHECK_EQUAL(g_sHoldingRegsBuf[ucCount + usStartAddress], sReceivedValue);
    }
}

TEST(Module, IllegalAddressInReadHoldingRegistersTest)
{
    uint8_t ucQueryBuf[QUERY_LEN] = {0, 0, 0, 0, 0, 6, 1, 3, 0, 0, 0, 16};
    
    memcpy(pucQuery, ucQueryBuf, QUERY_LEN);

    //function under test
    uint8_t usRecResponseLen      = mbap_ProcessRequest(pucQuery, QUERY_LEN, pucResponse);
    //expected response length
    uint8_t usExpectedResponseLen = MBT_EXCEPTION_PACKET_LEN;
    
    //check return value from test function
    CHECK_EQUAL(usExpectedResponseLen, usRecResponseLen);
    //check type of error from response
    CHECK_EQUAL(eILLEGAL_DATA_ADDRESS, pucResponse[MBT_BYTE_COUNT_OFFSET] );	
}

TEST(Module, IllegalFunctionCodeTest)
{
    uint8_t ucQueryBuf[QUERY_LEN] = {0, 0, 0, 0, 0, 6, 1, 10, 0, 0, 0, 11};
    
    memcpy(pucQuery, ucQueryBuf, QUERY_LEN);
    
    //function under test
    uint8_t usRecResponseLen      = mbap_ProcessRequest(pucQuery, QUERY_LEN, pucResponse);
    //expected response length
    uint8_t usExpectedResponseLen = MBT_EXCEPTION_PACKET_LEN;
    
    //check return value from test function
    CHECK_EQUAL(usExpectedResponseLen, usRecResponseLen);
    //check type of error from response
    CHECK_EQUAL(eILLEGAL_FUNCTION_CODE, pucResponse[MBT_BYTE_COUNT_OFFSET] );
}

TEST(Module, WriteSingleHoldingRegisterTest)
{
    uint8_t ucQueryBuf[QUERY_LEN] = {0, 0, 0, 0, 0, 6, 1, 6, 0, 1, 0, 200};
    int16_t sReceivedValue        = 0;
    int16_t sSentValue            = 0;
    
    memcpy(pucQuery, ucQueryBuf, QUERY_LEN);
    
    //function under test
    uint8_t usRecResponseLen      = mbap_ProcessRequest(pucQuery, QUERY_LEN, pucResponse);
    //expected response length
    uint8_t usExpectedResponseLen = MBAP_HEADER_LEN + 5;
    
    //check return value from test function
    CHECK_EQUAL(usExpectedResponseLen, usRecResponseLen);    
    
    //get holding register value
    sReceivedValue  = (int16_t)(pucResponse[REGISTER_VALUE_OFFSET] << 8);
    sReceivedValue |= (int16_t)(pucResponse[REGISTER_VALUE_OFFSET + 1]);
    sSentValue      = (int16_t)(ucQueryBuf[REGISTER_VALUE_OFFSET] << 8);
    sSentValue     |= (int16_t)(ucQueryBuf[REGISTER_VALUE_OFFSET + 1]);

    //check received value from sent value
    CHECK_EQUAL(sSentValue, sReceivedValue);
}

TEST(Module, IllegalDataValueInSingleHoldingRegisterWriteTest)
{
    uint8_t ucQueryBuf[QUERY_LEN] = {0, 0, 0, 0, 0, 6, 1, 6, 0, 1, 0, 201};
    
    memcpy(pucQuery, ucQueryBuf, QUERY_LEN);
  
    //function under test
    uint8_t usRecResponseLen      = mbap_ProcessRequest(pucQuery, QUERY_LEN, pucResponse);
    //expected response length
    uint8_t usExpectedResponseLen = MBT_EXCEPTION_PACKET_LEN;
    
    //check return value from test function
    CHECK_EQUAL(usExpectedResponseLen, usRecResponseLen);
    //check error type
    CHECK_EQUAL(eILLEGAL_DATA_VALUE, pucResponse[MBT_BYTE_COUNT_OFFSET] );
}

TEST(Module, ReadDiscreteInputsTest)
{
    uint8_t ucQueryBuf[QUERY_LEN] = {0, 0, 0, 0, 0, 6, 1, 2, 0, 0, 0, 3};
    uint8_t usExpectedResponseLen = 0;
    uint8_t ucByteCount           = 0;
    uint16_t usNumOfData          = 0;
    
    //set up test
    usNumOfData           = (uint16_t)(ucQueryBuf[NO_OF_DATA_OFFSET] << 8);
    usNumOfData          |= (uint16_t)(ucQueryBuf[NO_OF_DATA_OFFSET + 1]);
    usExpectedResponseLen = MBAP_HEADER_LEN  + (usNumOfData / 8) + 2;
    ucByteCount           = usNumOfData / 8;
    
    if (0 != ucQueryBuf[11])
    {
        usExpectedResponseLen = usExpectedResponseLen + 1;
        ucByteCount   = ucByteCount + 1;	
    }
    
    memcpy(pucQuery, ucQueryBuf, QUERY_LEN);
    
    //function under test
    uint8_t usRecResponseLen      = mbap_ProcessRequest(pucQuery, QUERY_LEN, pucResponse);
  
    //check return value from test function
    CHECK_EQUAL(usExpectedResponseLen, usRecResponseLen);
    //check byte count from response
    CHECK_EQUAL(ucByteCount, pucResponse[MBT_BYTE_COUNT_OFFSET]);
    
    //check received discrete inputs values from user data
    for (uint8_t ucCount = 0; ucCount < MAX_DISCRETE_INPUTS; ucCount++)
    {
        uint8_t  ucReceivedValue = 0;
        uint16_t usByteOffset    = 0;
        uint16_t usDiscreteBit   = 0;
        
        usByteOffset  = ucCount / 8 ;
        usDiscreteBit = ucCount - usByteOffset * 8;
        
        ucReceivedValue = (uint8_t)(pucResponse[MBT_DATA_VALUES_OFFSET + usByteOffset]);	

        CHECK_EQUAL(g_ucDiscreteInputsBuf[usByteOffset] & (1 << usDiscreteBit) , ucReceivedValue & (1 << usDiscreteBit));	
    }
}

TEST(Module, IllegalAddressInReadDiscreteInputsTest)
{
    uint8_t ucQueryBuf[QUERY_LEN] = {0, 0, 0, 0, 0, 6, 1, 2, 0, 0, 0, 16};
    
    memcpy(pucQuery, ucQueryBuf, QUERY_LEN);
    
    //function under test
    uint8_t usRecResponseLen      = mbap_ProcessRequest(pucQuery, QUERY_LEN, pucResponse);
    //expected response length
    uint8_t usExpectedResponseLen = MBT_EXCEPTION_PACKET_LEN;
    
    //check return value from test function
    CHECK_EQUAL(usExpectedResponseLen, usRecResponseLen);
    //check error type
    CHECK_EQUAL(eILLEGAL_DATA_ADDRESS, pucResponse[MBT_BYTE_COUNT_OFFSET] );	
}

TEST(Module, ReadCoilsTest)
{
    uint8_t ucQueryBuf[QUERY_LEN] = {0, 0, 0, 0, 0, 6, 1, 1, 0, 0, 0, 3};
    uint8_t usExpectedResponseLen = 0;
    uint8_t ucByteCount           = 0;
    uint16_t usNumOfData          = 0;
    
    //set up test
    usNumOfData            = (uint16_t)(ucQueryBuf[NO_OF_DATA_OFFSET] << 8);
    usNumOfData           |= (uint16_t)(ucQueryBuf[NO_OF_DATA_OFFSET + 1]);
    usExpectedResponseLen  = MBAP_HEADER_LEN  + (usNumOfData / 8) + 2;
    ucByteCount            = usNumOfData / 8;
    
    if (0 != ucQueryBuf[11])
    {
        usExpectedResponseLen = usExpectedResponseLen + 1;
        ucByteCount           = ucByteCount + 1;
    }
    
    memcpy(pucQuery, ucQueryBuf, QUERY_LEN);

    //function under test
    uint8_t usRecResponseLen      = mbap_ProcessRequest(pucQuery, QUERY_LEN, pucResponse);
    
    //check return value from test function
    CHECK_EQUAL(usExpectedResponseLen, usRecResponseLen);
    //check byte count from received response
    CHECK_EQUAL(ucByteCount, pucResponse[MBT_BYTE_COUNT_OFFSET]);
    
    //check received coils from user data
    for (uint8_t ucCount = 0; ucCount < MAX_COILS; ucCount++)
    {
        uint8_t  ucReceivedValue = 0;
        uint16_t usByteOffset    = 0;
        uint16_t usCoilsBit      = 0;
        
        usByteOffset = ucCount / 8 ;
        usCoilsBit   = ucCount - usByteOffset * 8;
        
        ucReceivedValue = (uint8_t)(pucResponse[MBT_DATA_VALUES_OFFSET + usByteOffset]);
        
        CHECK_EQUAL(g_ucCoilsBuf[usByteOffset] & (1 << usCoilsBit) , ucReceivedValue & (1 << usCoilsBit));
    }
}

TEST(Module, IllegalAddressInReadCoilsTest)
{
    uint8_t ucQueryBuf[QUERY_LEN] = {0, 0, 0, 0, 0, 6, 1, 1, 0, 0, 0, 16};
    
    memcpy(pucQuery, ucQueryBuf, QUERY_LEN);
    
    //function under test
    uint8_t usRecResponseLen      = mbap_ProcessRequest(pucQuery, QUERY_LEN, pucResponse);
    //expected response length
    uint8_t usExpectedResponseLen = MBT_EXCEPTION_PACKET_LEN;
    
    //check return value from test function
    CHECK_EQUAL(usExpectedResponseLen, usRecResponseLen);
    //check error type
    CHECK_EQUAL(eILLEGAL_DATA_ADDRESS, pucResponse[MBT_BYTE_COUNT_OFFSET] );	
}