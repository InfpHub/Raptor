CREATE TABLE IF NOT EXISTS CopyWriter
(
    LeafId  INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,
    Content TEXT    NOT NULL
);

CREATE TABLE IF NOT EXISTS Downloaded
(
    LeafId  INTEGER NOT NULL PRIMARY KEY,
    Name    TEXT,
    Path    TEXT,
    Created TEXT,
    Size    TEXT,
    UserId  TEXT    NOT NULL,
    SHA1    TEXT
);

CREATE TABLE IF NOT EXISTS Downloading
(
    LeafId      INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,
    Id          TEXT    NOT NULL,
    Name        TEXT,
    Path        TEXT,
    Created     TEXT,
    Size        TEXT,
    Partial     TEXT,
    Byte        TEXT,
    "Limit"     INTEGER,
    SHA1        TEXT,
    UserId      TEXT    NOT NULL,
    State       INTEGER NOT NULL,
    Parallel    INTEGER NOT NULL
);

CREATE TABLE IF NOT EXISTS Notice (
    LeafId  TEXT NOT NULL PRIMARY KEY,
    State  INTEGER NOT NULL
);

CREATE TABLE IF NOT EXISTS Timeline
(
    LeafId  INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,
    Date    TEXT    NOT NULL,
    Summary TEXT    NOT NULL,
    Active  INTEGER NOT NULL,
    Initial INTEGER NOT NULL
);

CREATE TABLE IF NOT EXISTS Uploaded
(
    LeafId  TEXT NOT NULL PRIMARY KEY,
    Name    TEXT,
    Path    TEXT,
    Created TEXT,
    Size    TEXT,
    UserId  TEXT NOT NULL
);

CREATE TABLE IF NOT EXISTS Uploading
(
    LeafId      TEXT    NOT NULL PRIMARY KEY,
    WorkerId    TEXT    NOT NULL,
    Name        TEXT,
    Created     TEXT,
    Path        TEXT,
    Size        TEXT,
    Transferred TEXT,
    Byte        TEXT,
    Space       TEXT,
    Rapid       INTEGER,
    UserId      TEXT    NOT NULL,
    Parent      TEXT    NOT NULL,
    State       INTEGER NOT NULL
);

CREATE TABLE IF NOT EXISTS User
(
    LeafId       TEXT NOT NULL PRIMARY KEY,
    Space        TEXT,
    Device       TEXT,
    NickName     TEXT,
    Avatar       TEXT,
    AccessToken  TEXT,
    RefreshToken TEXT,
    State        INTEGER,
    Private      TEXT,
    PublicKey    TEXT,
    Public       TEXT,
    Signature    TEXT,
    PrivateKey   TEXT
);

INSERT INTO CopyWriter (LeafId, Content)
VALUES (1, '我自狂歌空度日，飞扬跋扈为谁雄。');
INSERT INTO CopyWriter (LeafId, Content)
VALUES (2, '芝兰生于幽谷，不以无人而不芳。');
INSERT INTO CopyWriter (LeafId, Content)
VALUES (3, '世上本不缺女朋友，有的人多了，我的就没了。');
INSERT INTO CopyWriter (LeafId, Content)
VALUES (4, '此去关山万里，定不负云起之望。');
INSERT INTO CopyWriter (LeafId, Content)
VALUES (5, '大鹏一日同风起，扶摇直上九万里。');
INSERT INTO CopyWriter (LeafId, Content)
VALUES (6, '须知少时凌云志，曾许人间第一流。');
INSERT INTO CopyWriter (LeafId, Content)
VALUES (7, '正义都能迟到，为什么我上班不能迟到？');
INSERT INTO CopyWriter (LeafId, Content)
VALUES (8, '命运要你认清自己几斤几两的时候，不会提前打招呼。');
INSERT INTO CopyWriter (LeafId, Content)
VALUES (9, '你日渐平庸，甘于平庸，将继续平庸。');
INSERT INTO CopyWriter (LeafId, Content)
VALUES (10, '世界上本来是没路的，乱走的人多了，搞得我不知道怎么走了。');
INSERT INTO CopyWriter (LeafId, Content)
VALUES (11, '世上本没有女朋友。有的人多了，我妈觉得我也应该有。');
INSERT INTO CopyWriter (LeafId, Content)
VALUES (12, '迷失的人迷失了，相逢的人会再相逢。');
INSERT INTO CopyWriter (LeafId, Content)
VALUES (13, '喜欢一个人是藏不住的，多喜欢几个就能藏住了。');
INSERT INTO CopyWriter (LeafId, Content)
VALUES (14, '你一定要和她好好走下去，我坐车。');
INSERT INTO CopyWriter (LeafId, Content)
VALUES (15, '机会是留给有准备的人，没有机会别瞎准备。');
INSERT INTO CopyWriter (LeafId, Content)
VALUES (16, '安得广厦千万间，广厦一千万一间。');
INSERT INTO CopyWriter (LeafId, Content)
VALUES (17, '$3.53');
INSERT INTO CopyWriter (LeafId, Content)
VALUES (18, '放下个人素质，享受缺德人生。');
INSERT INTO CopyWriter (LeafId, Content)
VALUES (19, '拒绝精神内耗，有事直接发疯。');
INSERT INTO CopyWriter (LeafId, Content)
VALUES (20, '与其委屈自己，不如为难别人。');
INSERT INTO CopyWriter (LeafId, Content)
VALUES (21, '只要你肯吃苦，就有吃不完的苦。');

INSERT INTO Timeline (LeafId, Date, Summary, Active, Initial)
VALUES (1, '2024-02-14', '无人问津', 1, 1)