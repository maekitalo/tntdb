drop table foo;
drop sequence foo_seq;
create table foo
(
    a number not null,
    b integer not null
);
create sequence foo_seq;
create trigger foo_trigger
before insert on foo
for each row
    begin
        select foo_seq.nextval into :new.a from dual;
    end;
/

insert into foo (b) values (3);
insert into foo (b) values (7);

select * from foo;
