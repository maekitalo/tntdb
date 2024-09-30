create table myobjects
(
    id  serial not null primary key,
    a   int not null,
    b   varchar(64) not null
);

create table foo
(
    a   int not null primary key,
    b   varchar(64) not null
)
