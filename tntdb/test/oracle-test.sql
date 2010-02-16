
create sequence tntdbtest_seq;
create table tntdbtest
(
  id integer not null primary key,
  boolcol number(1),
  intcol integer,
  longcol integer,
  unsignedcol integer,
  unsignedlongcol integer,
  int32col integer,
  uint32col integer,
  int64col integer,
  uint64col integer,
  decimalcol decimal(16, 4),
  floatcol binary_float,
  doublecol binary_double,
  charcol char,
  stringcol varchar2(255),
  blobcol blob,
  datecol date,
  timecol date,
  datetimecol date
);
create trigger tntdbtest_trigger
before insert on tntdbtest
for each row
    begin
        select tntdbtest_seq.nextval into :new.id from dual;
    end;
/
