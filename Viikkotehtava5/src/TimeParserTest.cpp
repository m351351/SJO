#include <gtest/gtest.h>
#include "../TimeParser.h"

// Test suite: TimeParserTest
TEST(TimeParserTest, TestCaseCorrectTime) {

    // Test with correct time string
    char time_test[] = "000005";
    EXPECT_EQ(time_parse(time_test),5);

    char time_test2[] = "000105";
    EXPECT_EQ(time_parse(time_test2),65);
}

TEST(TimeParserTest, TestCaseIncorrectTime){
 
    char time_test3[] = "000000";
    EXPECT_EQ(time_parse(time_test3),TIME_VALUE_ERROR);

    char *time_test4 = NULL;
    EXPECT_EQ(time_parse(time_test4),TIME_ARRAY_ERROR);

    char time_test5[] = "000077";
    EXPECT_EQ(time_parse(time_test5),TIME_VALUE_ERROR); 

    char time_test6[] = "007700";
    EXPECT_EQ(time_parse(time_test6),TIME_VALUE_ERROR);
}
TEST(TimeParserTest, TestCaseIncorrectTimeValue){
    
    char time_test7[] = "aaaaaa";
    EXPECT_EQ(time_parse(time_test7),TIME_ARRAY_ERROR);

    char time_test8[] = "12345678";
    EXPECT_EQ(time_parse(time_test8),TIME_LEN_ERROR);

    char time_test9[] = "00000.";
    EXPECT_EQ(time_parse(time_test9),TIME_ARRAY_ERROR);

    char time_test10[] = "1234";
    EXPECT_EQ(time_parse(time_test10),TIME_LEN_ERROR);

}

TEST(TimeParserTest, TestCaseBirthTime){
    char time_test11[] = "174400";
    EXPECT_EQ(time_parse(time_test11),EASTEREGG);
}




TEST(liikennevalot, OikeitaMerkkeja){  
    char merkit[] = "RGYDT";
    EXPECT_EQ(liikennevalot(merkit), 0);
    

    char merkit2[] = "rgydt";
    EXPECT_EQ(liikennevalot(merkit2), 0);
    

    char merkit3[] = "rgy,1000";
    EXPECT_EQ(liikennevalot(merkit3), 0);

    }

TEST(liikennevalot, EiOikeitaMerkkeja){   

    char huonot_merkit[] = "ABABAB";
    EXPECT_EQ(liikennevalot(huonot_merkit), MARK_VALUE_ERROR);

    char huonot_merkit2[] = "RGY.555";
    EXPECT_EQ(liikennevalot(huonot_merkit), MARK_VALUE_ERROR);

    char huonot_merkit3[] = "rgy555"; //tässä vielä ongelmia
    EXPECT_EQ(liikennevalot(huonot_merkit3), MARK_VALUE_ERROR);

    }


//juttuja liittyen tarkistuksiin joissa syötteet ovat muotoa mmm,nnn
    // jossa m = merkki joka voi olla joku seuraavista: r, g, y, d tai d ja näitä mahtuu sequence arrayhin korkeintaan 20
    // ja n = numero, joka on sequenssin kestoaika millisekunteina


// https://google.github.io/googletest/reference/testing.html
// https://google.github.io/googletest/reference/assertions.html
