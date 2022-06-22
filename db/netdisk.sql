drop database if exists netdisk;
create database netdisk;
use netdisk;

# �û���
create table user (
  id int not null auto_increment,
  username varchar(20) not null,
  password varchar(20) not null,
  primary key (id)
);

# �ļ���
drop table if exists files;
create table files (
  id int primary key auto_increment,
  name varchar(255) not null,
  size int not null,
  path varchar(255) not null,
  link_count int not null,
  create_time datetime not null,
  update_time datetime not null,
);


# Ŀ¼��
drop table if exists directories;
create table directories (
  id int primary key auto_increment,
  name varchar(255) not null,
  par int not null,
  create_time datetime not null,
  update_time datetime not null,
);


# ��ϵ��
drop table if exists contacts;
create table contacts (
  id int primary key auto_increment,
  file int not null,
  directory int not null,
);