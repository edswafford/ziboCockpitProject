#! /bin/sh
# file: examples/party_test.sh

testEquality()
{
  assertEquals 1 1
}

testPartyLikeItIs1999()
{
  year=`date '+%Y'`
  assertEquals "It's not 1999 :-(" \
      '1999' "${year}"
}

# load shunit2
. "../shunit2-2.1.6/src/shunit2"