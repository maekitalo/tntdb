create table foo
(
    a integer not null primary key auto_increment,
    b integer not null
);

insert into foo (b) values (3);
insert into foo (b) values (7);

select * from foo;
