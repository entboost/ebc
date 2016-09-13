
CREATE DATABASE KV;
USE KV;

CREATE TABLE keyvalue_t
(
	namespace		VARCHAR(32),
	key					VARCHAR(64),
	value				VARCHAR(1000)
);
