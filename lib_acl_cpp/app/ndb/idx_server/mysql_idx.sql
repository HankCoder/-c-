CREATE DATABASE `db_name`;
USE `db_name`;

CREATE TABLE `tbl_dat` (
	`id_database` int(10) unsigned NOT NULL COMMENT '���ݿ�����ID',
	`id_table` int(10) unsigned NOT NULL COMMENT '���ݱ�����ID',
	`id_index` int(10) unsigned NOT NULL '������������ID',
	`key` varchar(220) NOT NULL '�����ֶ�ֵ',
	`id_dat`, bigint(20) unsigned NOT NULL COMMENT '�洢ΨһID��',
	PRIMARY KEY(`id_database`, `id_table`, `id_index`, `key`) USING BTREE,
	KEY `Index_id_dat` (`id_dat`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

��

CREATE TABLE `tbl_dat` (
	`key` varchar(256) NOT NULL COMMENT '�� id_database-id_table-id_index-id:key ƴ�Ӷ���',
	`id_dat`, bigint(20) unsigned NOT NULL COMMENT '�洢ΨһID��',
	PRIMARY KEY(`key`) USING BTREE,
	KEY `Index_id_dat` (`id_dat`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
