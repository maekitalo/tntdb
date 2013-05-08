--drop table tntdbtest;
--drop sequence tntdbtest_seq;

create sequence tntdbtest_seq;
create table tntdbtest
(
  id integer not null primary key,
  boolcol number(1),
  intcol number(10),
  longcol number(24),
  unsignedcol number(10),
  unsignedlongcol number(24),
  int32col number(10),
  uint32col number(10),
  int64col number(24),
  uint64col number(24),
  decimalcol decimal(24, 4),
  floatcol binary_float,
  doublecol binary_double,
  charcol char,
  stringcol varchar2(255),
  blobcol blob,
  datecol date,
  timecol timestamp,
  datetimecol timestamp
);
create trigger tntdbtest_trigger
before insert on tntdbtest
for each row
    begin
        select tntdbtest_seq.nextval into :new.id from dual;
    end;
/
