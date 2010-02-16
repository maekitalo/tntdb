create table tntdbtest
(
  id integer not null primary key auto_increment,
  boolcol boolean,
  intcol integer,
  longcol bigint,
  unsignedcol bigint,
  unsignedlongcol decimal(24),
  int32col integer,
  uint32col bigint,
  int64col bigint,
  uint64col decimal(24),
  decimalcol decimal(16, 4),
  floatcol real,
  doublecol double precision,
  charcol char,
  stringcol varchar(255),
  blobcol blob,
  datecol date,
  timecol time,
  datetimecol timestamp
);
