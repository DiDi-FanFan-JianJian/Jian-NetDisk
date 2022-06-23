drop database if exists netdisk;
create database netdisk;
use netdisk;

# �û���
drop table if exists users;
create table `users` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `username` varchar(20) NOT NULL,
  `password` varchar(20) NOT NULL,
  PRIMARY KEY (`id`)
);

insert into users(username, password) values('admin', 'admin');
insert into users(username, password) values('dino', '1234565');

# �ļ���
drop table if exists files;
create table `files` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `name` varchar(20) NOT NULL, # �ļ���
  `size` int(11) NOT NULL, # �ļ���С
  `md5` varchar(32) NOT NULL, # �ļ�md5
  `path` varchar(100) NOT NULL, # �ļ�·��
  `count` int(11) NOT NULL, # ������
  PRIMARY KEY (`id`)
);

# Ŀ¼��
drop table if exists dirs;
create table `dirs` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `name` varchar(20) NOT NULL, # Ŀ¼��
  `uid` int(11) NOT NULL, # �û�id
  `pid` int(11) NOT NULL, # ��Ŀ¼id
  `create_time` datetime NOT NULL, # ����ʱ��
  `update_time` datetime NOT NULL, # ����ʱ��
  PRIMARY KEY (`id`)
);

# �ļ�Ŀ¼��ϵ�� ������Ϊ���û����ļ�
drop table if exists file_dir;
create table `file_dir` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `fid` int(11) NOT NULL, # �ļ�id
  `did` int(11) NOT NULL, # Ŀ¼id
  `create_time` datetime NOT NULL, # ����ʱ��
  `update_time` datetime NOT NULL, # ����ʱ��
  PRIMARY KEY (`id`)
);