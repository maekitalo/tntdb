create sequence tntdbtest_seq;
create table tntdbtest
(
  id integer not null primary key default nextval('tntdbtest_seq'),
  boolcol boolean,
  shortcol integer,
  intcol integer,
  longcol bigint,
  unsignedshortcol integer,
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
  stringcol text,
  blobcol bytea,
  datecol date,
  timecol time,
  datetimecol timestamp
);
alter sequence tntdbtest_seq owned by tntdbtest.id;
