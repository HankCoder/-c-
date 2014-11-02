CREATE DATABASE `db_ctl`;
USE `db_ctl`;

/* �ñ����� ���ݿ�|���ݱ�|���� ���ַ�������������ֵ��ӳ��� */
CREATE TABLE `tbl_name_type` (
	`id` int(10) unsigned NOT NULL AUTO_INCREMENT COMMENT '��Ӧ name ��ID��',
	`name` varchar(64) NOT NULL COMMENT '���ݿ�|���ݱ�|����������֮һ',
	`type` tinyint(1) unsigned NOT NULL COMMENT '������¼����������: 0-���ݿ⣬1-��2-����',
	PRIMARY KEY(`id`) USING BTREE,
	UNIQUE KEY `Index_name_type` (`name`, `type`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

/* ������������Ϣ����¼������ID�ţ���ַ���洢��¼�� */
CREATE TABLE `tbl_idx_host` (
	`id` int(10) unsigned NOT NULL AUTO_INCREMENT COMMENT '����������Ψһ��ʶ��',
	`addr` varchar(32) NOT NULL COMMENT '������ַ��IP:PORT �����׽ӿ�',
	`count` bigint(20) NOT NULL COMMENT '�Ѿ��洢���ܼ�¼��',
	PRIMARY KEY(`id`) USING BTREE,
	UNIQUE KEY `Index_addr` (`addr`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

/* ���ݷ�������Ϣ����¼������ID�ţ���ַ���洢��¼�� */
CREATE TABLE `tbl_dat_host` (
	`id` int(10) unsigned NOT NULL AUTO_INCREMENT COMMENT '���ݷ�����Ψһ��ʶ��',
	`addr` varchar(32) NOT NULL COMMENT '������ַ��IP:PORT �����׽ӿ�',
	`priority` int(10) NOT NULL COMMENT '����������Ȩ�أ�ֵԽ�߱�ʾ����Խ��',
	`count` bigint(20) NOT NULL COMMENT '�Ѿ��洢���ܼ�¼��',
	PRIMARY KEY(`id`) USING BTREE,
	UNIQUE KEY `Index_addr` (`addr`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

/* ���������ݿ��е��������ݿ������б� */
CREATE TABLE `tbl_db_host` (
	`id_db` int(10) unsigned NOT NULL COMMENT '���ݿ�ID�ţ���Ӧ tbl_name �� type ����ֵΪ 0 �� id',
	`id_idx_host` int(10) unsigned NOT NULL COMMENT '����������ID�ţ���Ӧ tbl_idx_host �е� id',
	`count` bigint(20) unsigned NOT NULL COMMENT '�����ݿ��������еļ�¼����',
	PRIMARY KEY(`id_db`, `id_idx_host`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

/* ���������ݿ��е����ݱ��б� */
CREATE TABLE `tbl_db_tbl` (
	`id_db` int(10) unsigned NOT NULL COMMENT '���ݿ�ID�ţ���Ӧ tbl_name �� type ����ֵΪ 0 �� id',
	`id_tbl` int(10) unsigned NOT NULL COMMENT '���ݱ���ID�ţ���Ӧ tbl_name �� type ����ֵΪ 1 �� id',
	`count` bigint(20) NOT NULL COMMENT '�����ݱ��м�¼����',
	PRIMARY KEY(`db_name`, `tbl_name`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

/* ���ݱ����������ϱ� */
CREATE TABLE `tbl_tbl_idx` (
	`id_idx` int(10) unsigned NOT NULL COMMENT '���ݱ�������ID�ţ���Ӧ tbl_name �� type ����ֵΪ 2 �� id',
	`id_db` int(10) unsigned NOT NULL COMMNET '���ݿ�ID�ţ���Ӧ tbl_name �� type ����ֵΪ 0 �� id',
	`id_tbl` int(10) unsigned NOT NULL COMMENT '���ݱ�ID�ţ���Ӧ tbl_name �� type ����ֵΪ 1 �� id',
	`unique` tinyint(1) unsigned NOT NULL COMMENT '�����������ݱ����Ƿ���Ψһ��������',
	`type` tinyint(1) unsigned NOT NULL COMMENT '�������ͣ�0 - �ַ�����1 - �����ͣ�2 - 16λ������3 - 32λ������4 - 64λ����',
	PRIMARY KEY(`id_idx`, `id_db`, `id_tbl`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
