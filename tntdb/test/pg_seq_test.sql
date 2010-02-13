create sequence foo_seq;
create table foo
(
    a integer not null default nextval('foo_seq'),
    b integer not null
);
alter SEQUENCE foo_seq OWNED BY foo.a;

insert into foo (b) values (3);
insert into foo (b) values (7);

select * from foo;
