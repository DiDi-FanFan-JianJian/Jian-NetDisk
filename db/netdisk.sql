drop database if exists netdisk;
create database netdisk;
use netdisk;

# 用户表
drop table if exists users;
create table `users` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `username` varchar(20) NOT NULL,
  `password` varchar(20) NOT NULL,
  PRIMARY KEY (`id`)
);

insert into users(username, password) values('admin', 'admin');
insert into users(username, password) values('dino', '1234565');

# 文件表
drop table if exists files;
create table `files` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `name` varchar(20) NOT NULL, # 文件名
  `size` int(11) NOT NULL, # 文件大小
  `md5` varchar(32) NOT NULL, # 文件md5
  `path` varchar(100) NOT NULL, # 文件路径
  `count` int(11) NOT NULL, # 链接数
  PRIMARY KEY (`id`)
);

# 目录表
drop table if exists dirs;
create table `dirs` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `name` varchar(20) NOT NULL, # 目录名
  `uid` int(11) NOT NULL, # 用户id
  `pid` int(11) NOT NULL, # 父目录id
  `create_time` datetime NOT NULL, # 创建时间
  `update_time` datetime NOT NULL, # 更新时间
  PRIMARY KEY (`id`)
);

# 文件目录联系表 可以视为是用户的文件
drop table if exists file_dir;
create table `file_dir` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `fid` int(11) NOT NULL, # 文件id
  `did` int(11) NOT NULL, # 目录id
  `create_time` datetime NOT NULL, # 创建时间
  `update_time` datetime NOT NULL, # 更新时间
  PRIMARY KEY (`id`)
);