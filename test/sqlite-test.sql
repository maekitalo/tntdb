create table tntdbtest
(
  id integer not null primary key autoincrement,
  boolcol boolean,
  shortcol integer,
  intcol integer,
  longcol integer,
  unsignedshortcol integer,
  unsignedcol integer,
  unsignedlongcol integer,
  int32col integer,
  uint32col integer,
  int64col integer,
  uint64col integer,
  decimalcol decimal(10, 4),
  floatcol real,
  doublecol real,
  charcol char,
  stringcol text,
  blobcol blob,
  datecol date,
  timecol time,
  datetimecol datetime
);
