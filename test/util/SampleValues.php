<?php

class SampleValues {

    public static $DECIMAL;
    public static $TIMESTAMP = 123456789;
    public static $BIGINT = 64;
    public static $INTEGER = 32;
    public static $SMALLINT = 16;
    public static $TINYINT = 8;
    public static $FLOAT = 3.14;
    public static $VARCHAR = 'varchar';

    public static function init() {
        self::$DECIMAL = new Decimal('3.1415');
    }

}
SampleValues::init();
