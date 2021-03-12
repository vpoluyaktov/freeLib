/* Disable foreign keys */
PRAGMA foreign_keys = 'off';

/* Begin transaction */
BEGIN;

/* Database properties */
PRAGMA auto_vacuum = 0;
PRAGMA encoding = 'UTF-8';
PRAGMA page_size = 32768;

/* Drop table [tag] */
DROP TABLE IF EXISTS [main].[tag];

/* Table structure [tag] */
CREATE TABLE [main].[tag](
  [id] INTEGER PRIMARY KEY NOT NULL, 
  [color] TEXT DEFAULT (NULL), 
  [name] TEXT
);

/* Drop table [lib] */
DROP TABLE IF EXISTS [main].[lib];

/* Table structure [lib] */
CREATE TABLE [main].[lib](
  [id] INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, 
  [name] TEXT, 
  [path] TEXT, 
  [inpx] TEXT, 
  [firstAuthor] BOOL, 
  [woDeleted] BOOL,
  [currentTab] INTEGER,
  [currentAuthor] INTEGER, 
  [currentSeria] INTEGER, 
  [currentGenre] INTEGER,
  [currentGroup] INTEGER,
  [currentBookForAuthor] INTEGER, 
  [currentBookForSeria] INTEGER, 
  [currentBookForGenre] INTEGER,
  [currentBookForGroup] INTEGER,
  [currentSearchingFilter] TEXT,
  [currentTag] INTEGER,
  [currentBookLanguage] TEXT NOT NULL DEFAULT '*'
);

/* Drop table [author] */
DROP TABLE IF EXISTS [main].[author];

/* Table structure [author] */
CREATE TABLE [main].[author](
  [id] INTEGER PRIMARY KEY, 
  [LastName] TEXT,
  [FirstName] TEXT,
  [MiddleName] TEXT,
  [NickName] TEXT,
  [id_lib] INTEGER REFERENCES [lib]([id]),
  [tag] INTEGER REFERENCES [tag]([id])
);
CREATE INDEX [main].[author_sort]
ON [author](
  [LastName] ASC, 
  [FirstName] ASC, 
  [MiddleName] ASC
);

/* Drop table [seria] */
DROP TABLE IF EXISTS [main].[seria];

/* Table structure [seria] */
CREATE TABLE [main].[seria](
  [id] INTEGER PRIMARY KEY,
  [name] TEXT, 
  [id_lib] INTEGER REFERENCES [lib]([id]), 
  [tag] INTEGER REFERENCES [tag]([id])
);
CREATE INDEX [main].[seria_id] ON [seria]([id] ASC);
CREATE INDEX [main].[seria_name]
ON [seria](
  [name] ASC,
  [id_lib] ASC
);

/* Drop table [groups] */
DROP TABLE IF EXISTS [main].[groups];

/* Table structure [groups] */
CREATE TABLE [main].[groups](
  [id] INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
  [name] TEXT NOT NULL,
  [id_lib] INTEGER REFERENCES [lib]([id]),
  [blocked] BOOL DEFAULT FALSE,
  [blocked_name] TEXT,
  [icon] BLOB
);
CREATE INDEX [main].[group_name] ON [groups]([name] ASC);

/* Drop table [book] */
DROP TABLE IF EXISTS [main].[book];

/* Table structure [book] */
CREATE TABLE [main].[book](
  [id] INTEGER PRIMARY KEY, 
  [name] TEXT, 
  [star] INTEGER DEFAULT 0, 
  [id_seria] INTEGER REFERENCES [seria]([id]), 
  [num_in_seria] INTEGER DEFAULT 0, 
  [language] TEXT, 
  [id_lib] INTEGER REFERENCES [lib]([id]), 
  [file] TEXT, 
  [size] INTEGER, 
  [deleted] BOOL DEFAULT FALSE, 
  [date] DATETIME, 
  [format] TEXT, 
  [keys] TEXT, 
  [id_inlib] INTEGER, 
  [archive] TEXT, 
  [first_author_id] INTEGER, 
  [tag] INTEGER REFERENCES [tag]([id]),
  [readed] BOOL DEFAULT FALSE
);
CREATE INDEX [main].[book_seria] ON [book]([id_seria] ASC);
CREATE INDEX [main].[book_file]
ON [book](
  [id_lib], 
  [file], 
  [archive]
);
CREATE INDEX [main].[book_first_author] ON [book]([first_author_id] ASC);

/* Drop table [book_author] */
DROP TABLE IF EXISTS [main].[book_author];

/* Table structure [book_author] */
CREATE TABLE [main].[book_author](
  [id_book] INTEGER REFERENCES [book]([id]), 
  [id_author] INTEGER REFERENCES [author]([id]), 
  [id_lib] INTEGER REFERENCES [lib]([id]), 
  [language] TEXT, 
  PRIMARY KEY([id_book], [id_author], [id_lib])
);
CREATE INDEX [main].[book_author_id_book] ON [book_author]([id_book] ASC);
CREATE INDEX [main].[book_author_id_author] ON [book_author]([id_author]);

/* Drop table [book_genre] */
DROP TABLE IF EXISTS [main].[book_genre];

/* Table structure [book_genre] */
CREATE TABLE [main].[book_genre](
  [id_book] INTEGER, 
  [id_genre] INTEGER, 
  [id_lib] INTEGER REFERENCES [lib]([id]), 
  [language] TEXT, 
  PRIMARY KEY([id_book], [id_genre], [id_lib])
);
CREATE INDEX [main].[book_genre_id_book] ON [book_genre]([id_book] ASC);
CREATE INDEX [main].[book_genre_id] ON [book_genre]([id_genre] ASC);

/* Drop table [genre] */
DROP TABLE IF EXISTS [main].[genre];

/* Table structure [genre] */
CREATE TABLE [main].[genre](
  [id] INTEGER PRIMARY KEY,
  [name] TEXT,
  [name_en] TEXT,
  [id_parent] INTEGER,
  [sort_index] INTEGER,
  [code] TEXT DEFAULT (NULL)
);

/* Drop table [book_group] */
DROP TABLE IF EXISTS [main].[book_group];

/* Table structure [book_group] */
CREATE TABLE IF NOT EXISTS [main].[book_group] (
  [book_id] INTEGER NOT NULL,
  [group_id] INTEGER NOT NULL,
  [id_lib] INTEGER NOT NULL, 
  PRIMARY KEY ([book_id], [group_id], [id_lib]),
  CONSTRAINT [fk_book_group_to_books]
     FOREIGN KEY (book_id) REFERENCES [book] (id)
        ON DELETE CASCADE ON UPDATE CASCADE,
  CONSTRAINT [fk_book_group_to_groups]
     FOREIGN KEY (group_id) REFERENCES [groups] (id)
        ON DELETE CASCADE ON UPDATE CASCADE,
  CONSTRAINT [fk_book_group_to_lib]
     FOREIGN KEY (id_lib) REFERENCES [lib] (id)
        ON DELETE CASCADE ON UPDATE CASCADE
);

/* Drop table [params] */
DROP TABLE IF EXISTS [main].[params];

/* Table structure [params] */
CREATE TABLE [main].[params](
  [id] INTEGER PRIMARY KEY, 
  [name] TEXT NOT NULL UNIQUE, 
  [value] TEXT
);

/* Table data [tag] Record count: 7 */
INSERT INTO [tag]([rowid], [id], [color], [name]) VALUES(1, 1, '#ff888b', 'Красный');
INSERT INTO [tag]([rowid], [id], [color], [name]) VALUES(2, 2, '#fec84f', 'Оранжевый');
INSERT INTO [tag]([rowid], [id], [color], [name]) VALUES(3, 3, '#fdf14b', 'Желтый');
INSERT INTO [tag]([rowid], [id], [color], [name]) VALUES(4, 4, '#baf144', 'Зеленый');
INSERT INTO [tag]([rowid], [id], [color], [name]) VALUES(5, 5, '#97d2fd', 'Синий');
INSERT INTO [tag]([rowid], [id], [color], [name]) VALUES(6, 6, '#edb2fd', 'Лиловый');
INSERT INTO [tag]([rowid], [id], [color], [name]) VALUES(7, 7, '#c8c8c8', 'Серый');

/* Table data [lib] Record count: 0 */

/* Table data [author] Record count: 0 */

/* Table data [seria] Record count: 0 */

/* Table data [book] Record count: 0 */

/* Table data [book_author] Record count: 0 */

/* Table data [book_genre] Record count: 0 */

/* Table data [genre] Record count: 474 */
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(1, 1, 'Военное дело', 'Military', 0, null, null);
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(2, 2, 'Боевые искусства', 'Military Arts', 1, 10, 'military_arts');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(3, 3, 'Военная документалистика и аналитика', 'Military Documentaries And Analytics', 1, 20, 'nonf_military');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(4, 4, 'Военная история', 'Military History', 1, 30, 'military_history');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(5, 5, 'Военная техника и вооружение', 'Weapon', 1, 40, 'military_weapon');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(6, 6, 'Военное дело: прочее', 'Military Other', 1, 50, 'military');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(7, 7, 'Военные мемуары', 'Military Memoirs', 1, 60, 'military_memoirs');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(8, 8, 'Спецслужбы', 'Military Special', 1, 70, 'military_special');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(9, 9, 'Деловая литература', 'Economy, Business', 0, null, null);
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(10, 10, 'Банковское дело', 'Banking', 9, 10, 'banking');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(11, 11, 'Бухучет и аудит', 'Accounting', 9, 20, 'accounting');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(12, 12, 'Внешняя торговля', 'Global Economy', 9, 30, 'global_economy');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(13, 13, 'Деловая литература: прочее', 'Business Literature', 9, 40, 'sci_business');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(14, 14, 'Делопроизводство', 'Paper Work', 9, 50, 'paper_work');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(15, 15, 'Корпоративная культура', 'Corporate Culture', 9, 60, 'org_behavior');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(16, 16, 'Личные финансы', 'Personal Finance', 9, 70, 'personal_finance');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(17, 17, 'Малый бизнес', 'Small Business', 9, 80, 'small_business');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(18, 18, 'Маркетинг, PR, реклама', 'Marketing, PR, Adv', 9, 90, 'marketing');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(19, 19, 'Недвижимость', 'Real Estate', 9, 100, 'real_estate');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(20, 20, 'О бизнесе популярно', 'Popular Business', 9, 110, 'popular_business');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(21, 21, 'Отраслевые издания', 'Industries', 9, 120, 'industries');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(22, 22, 'Поиск работы, карьера', 'Job Hunting', 9, 130, 'job_hunting');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(23, 23, 'Практическая экономика', 'Economics', 9, 140, 'economics');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(24, 24, 'Справочная деловая литература', 'economics_ref', 9, 150, 'economics_ref');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(25, 25, 'Торговля', 'Trade', 9, 160, 'trade');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(26, 26, 'Управление, подбор персонала', 'Management', 9, 170, 'management');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(27, 27, 'Ценные бумаги, инвестиции', 'Stock', 9, 180, 'stock');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(28, 28, 'Детективы и триллеры', 'Detectives & Thrillers', 0, null, null);
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(29, 29, 'Археологический триллер', 'Archaeological Thriller', 28, 10, 'thriller_archaeological');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(30, 30, 'Боевик', 'Action', 28, 20, 'det_action');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(31, 31, 'Дамский детектив', 'Cozy Mysteries', 28, 30, 'det_cozy');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(32, 32, 'Детективы: прочее', 'Detectives', 28, 40, 'detective');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(33, 33, 'Зарубежный детектив', 'Foreign Detectives', 28, 50, 'foreign_detective');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(34, 34, 'Иронический детектив', 'Ironical Detectives', 28, 60, 'det_irony');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(35, 35, 'Исторический детектив', 'Historical Detectives', 28, 70, 'det_history');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(36, 36, 'Классический детектив', 'Classical Detectives', 28, 80, 'det_classic');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(37, 37, 'Криминальный детектив', 'Crime Detectives', 28, 90, 'det_crime');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(38, 38, 'Крутой детектив', 'Hard-boiled', 28, 100, 'det_hard');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(39, 39, 'Маньяки', 'Maniacs', 28, 110, 'det_maniac');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(40, 40, 'Медицинский триллер', 'Medical Thriller', 28, 120, 'thriller_medical');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(41, 41, 'Политический детектив', 'Political Detectives', 28, 130, 'det_political');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(42, 42, 'Полицейский детектив', 'Police Stories', 28, 140, 'det_police');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(43, 43, 'Российский детектив', 'Russian Detectives', 28, 150, 'det_rus');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(44, 44, 'Советский детектив', 'Soviet Detectives', 28, 160, 'det_su');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(45, 45, 'Техно триллер', 'Techno Thriller', 28, 170, 'thriller_techno');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(46, 46, 'Триллер', 'Thrillers', 28, 180, 'thriller');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(47, 47, 'Шпионский детектив', 'Espionage Detectives', 28, 190, 'det_espionage');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(48, 48, 'Юридический триллер', 'Legal Thriller', 28, 200, 'thriller_legal');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(49, 49, 'Детская литература', 'Childrens', 0, null, null);
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(50, 50, 'Детская литература: прочее', 'Childrens Other', 49, 10, 'children');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(51, 51, 'Детская образовательная литература', 'Childrens Educational', 49, 20, 'child_education');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(52, 52, 'Детская остросюжетная литература', 'Childrens Detectives & Thrillers', 49, 30, 'child_det');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(53, 53, 'Детская проза', 'Childrens Prose', 49, 40, 'child_prose');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(54, 54, 'Детские приключения', 'Childrens Adventures', 49, 50, 'child_adv');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(55, 55, 'Детские стихи', 'Childrens Verses', 49, 60, 'child_verse');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(56, 56, 'Детские ужастики', 'Childrens Horror', 49, 70, 'child_horror');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(57, 57, 'Детская фантастика', 'Childrens Science Fiction', 49, 80, 'child_sf');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(58, 58, 'Детское фэнтези', 'Childrens Fantasy', 49, 90, 'child_fantasy');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(59, 59, 'Детский фольклор', 'Child Folklore', 49, 100, 'child_folklore');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(60, 60, 'Зарубежная детская литература', 'Foreign Literature For Children', 49, 110, 'foreign_children');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(61, 61, 'Классическая детская литература', 'Classical Childrens Literature', 49, 120, 'child_classical');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(62, 62, 'Подростковая литература', 'Young-adult Fiction', 49, 130, 'ya');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(63, 63, 'Русская детская литература', 'Russian Childrens Literature', 49, 140, 'child_rus');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(64, 64, 'Русские сказки для детей', 'Russian Fairy Tales', 49, 150, 'child_tale_rus');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(65, 65, 'Сказки для детей', 'Fairy Tales', 49, 160, 'child_tale');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(66, 66, 'Советская детская литература', 'Soviet Childrens Literature', 49, 170, 'child_su');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(67, 67, 'Документальная литература', 'Nonfiction', 0, null, null);
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(68, 68, 'Беседы и интервью', 'Interview', 67, 10, 'interview');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(69, 69, 'Биографии и мемуары', 'Biography & Memoirs', 67, 20, 'nonf_biography');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(70, 70, 'Документальная литература', 'Nonfiction Other', 67, 30, 'nonfiction');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(71, 71, 'Заметки натуралиста', 'Naturalist Notes', 67, 40, 'naturalist_notes');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(72, 72, 'Записки путешественника', 'Travel Notes', 67, 50, 'travel_notes');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(73, 73, 'Научпоп', 'Science Popular', 67, 60, 'sci_popular');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(74, 74, 'Публицистика', 'Publicism', 67, 70, 'nonf_publicism');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(75, 75, 'Домоводство (Дом и семья)', 'Home & Family', 0, null, null);
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(76, 76, 'Дети. Книги для родителей', 'Child', 75, 10, 'home_child');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(77, 77, 'Дом и семья: прочее', 'Home Other', 75, 20, 'home');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(78, 78, 'Домашние животные', 'Pets', 75, 30, 'home_pets');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(79, 79, 'Домоводство', 'Housekeeping', 75, 40, 'home_housekeeping');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(80, 80, 'Здоровье', 'Health', 75, 50, 'home_health');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(81, 81, 'Красота', 'Beauty', 75, 60, 'home_beauty');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(82, 82, 'Кулинария', 'Cooking', 75, 70, 'home_cooking');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(83, 83, 'Развлечения', 'Entertaining', 75, 80, 'home_entertain');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(84, 84, 'Семейные отношения', 'Family', 75, 90, 'family');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(85, 85, 'Эротика и секс', 'Erotica & Sex', 75, 100, 'home_sex');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(86, 86, 'Драматургия', 'Dramaturgy', 0, null, null);
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(87, 87, 'Античная драма', 'Antique Drama', 86, 10, 'drama_antique');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(88, 88, 'Водевиль, буффонада', 'Vaudeville', 86, 20, 'vaudeville');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(89, 89, 'Драма', 'Drama', 86, 30, 'drama');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(90, 90, 'Драматургия', 'Dramaturgy', 86, 40, 'dramaturgy');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(91, 91, 'Зарубежная классическая драматургия', 'Foreign Dramaturgy', 86, 50, 'dramaturgy_for_classic');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(92, 92, 'Зарубежная современная драматургия', 'Contemporary Foreign Dramaturgy', 86, 60, 'foreign_dramaturgy');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(93, 93, 'Киносценарий', 'Screenplays', 86, 70, 'screenplays');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(94, 94, 'Комедия', 'Comedy', 86, 80, 'comedy');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(95, 95, 'Мистерия', 'Mystery', 86, 90, 'mystery');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(96, 96, 'Русская классическая драматургия', 'Russian Dramaturgy', 86, 100, 'dramaturgy_rus_classic');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(97, 97, 'Русская современная драматургия', 'Contemporary Russian Dramaturgy', 86, 110, 'dramaturgy_rus');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(98, 98, 'Советская драматургия', 'Soviet Dramaturgy', 86, 120, 'dramaturgy_su');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(99, 99, 'Сценарий', 'Scenarios', 86, 130, 'scenarios');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(100, 100, 'Трагедия', 'Tragedy', 86, 140, 'tragedy');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(101, 101, 'История', 'History', 0, null, null);
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(102, 102, 'История', 'History', 101, 10, 'sci_history');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(103, 103, 'История первобытного общества', 'History of Primitive Society', 101, 20, 'sci_history_0');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(104, 104, 'История древнего мира', 'Ancient World History', 101, 30, 'sci_history_4');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(105, 105, 'История средних веков', 'Medieval History', 101, 40, 'sci_history_15');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(106, 106, 'История нового времени', 'History of Modern Times', 101, 50, 'sci_history_18');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(107, 107, 'Новейшая история', 'Newest History', 101, 60, 'sci_history_20');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(108, 108, 'Современная история', 'Contemporary History', 101, 70, 'sci_history_21');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(109, 109, 'Компьютеры и Интернет', 'Computers & Internet', 0, null, null);
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(110, 110, 'CAD, CAM и CAE системы', 'CAD, CAM and CAE', 109, 10, 'comp_soft_cad');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(111, 111, 'Web-дизайн', 'Web Design', 109, 20, 'comp_www_design');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(112, 112, 'Базы данных', 'Databases', 109, 30, 'comp_db');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(113, 113, 'Графика. Дизайн. Мультимедиа', 'Design', 109, 40, 'comp_design');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(114, 114, 'Интернет', 'Internet', 109, 50, 'comp_www');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(115, 115, 'Компьютерная безопасность', 'Security', 109, 60, 'comp_security');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(116, 116, 'Компьютерное железо', 'Hardware', 109, 70, 'comp_hard');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(117, 117, 'Околокомпьютерная литература', 'Computers Other', 109, 80, 'computers');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(118, 118, 'Офисные приложения', 'Office Software', 109, 90, 'comp_soft_office');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(119, 119, 'Программы', 'Software', 109, 100, 'comp_soft');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(120, 120, 'Сети', 'Networking', 109, 110, 'comp_osnet');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(121, 121, 'Учебники и самоучители по компьютеру', 'Computers Textbook', 109, 120, 'tbg_computers');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(122, 122, 'Хакерство', 'Hacking', 109, 130, 'comp_hacking');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(123, 123, 'Цифровая обработка сигналов', 'Digital Signal Processing', 109, 140, 'comp_dsp');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(124, 124, 'Компьютеры: Операционные системы', 'OS', 0, null, null);
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(125, 125, 'Android', 'Android', 124, 10, 'comp_os_android');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(126, 126, 'FreeDOS', 'FreeDOS', 124, 20, 'comp_os_freedos');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(127, 127, 'Linux', 'Linux', 124, 30, 'comp_os_linux');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(128, 128, 'MacOS', 'MacOS', 124, 40, 'comp_os_macos');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(129, 129, 'MS-DOS', 'MS-DOS', 124, 50, 'comp_os_msdos');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(130, 130, 'OS/2', 'OS/2', 124, 60, 'comp_os_os2');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(131, 131, 'Unix', 'Unix', 124, 70, 'comp_os_unix');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(132, 132, 'Windows', 'Windows', 124, 80, 'comp_os_windows');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(133, 133, 'Прочие ОС', 'Other OS', 124, 90, 'comp_os');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(134, 134, 'Компьютеры: Разработка ПО', 'Soft Development', 0, null, null);
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(135, 135, 'Алгоритмы', 'Algorithms', 134, 10, 'comp_soft_dev_alg');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(136, 136, 'Искусственный интеллект', 'Artifical Intellegence', 134, 20, 'comp_dv_ai');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(137, 137, 'Крэкинг и реверсинжиниринг', 'Craking And Reverse Engineering', 134, 30, 'comp_soft_dev_craking');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(138, 138, 'Менеджмент ПО', 'Management', 134, 40, 'comp_soft_dev_man');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(139, 139, 'Объектно-ориентированное программирование', 'OOP', 134, 50, 'comp_soft_dev_oop');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(140, 140, 'Отладка и тестирование ПО', 'Debuging', 134, 60, 'comp_soft_dev_debug');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(141, 141, 'Программирование графики', 'Graphic Programming', 134, 70, 'comp_soft_dev_graphic');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(142, 142, 'Программирование игр', 'Games Programming', 134, 80, 'comp_soft_dev_games');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(143, 143, 'Разработка ПО: прочее', 'Soft Development Other', 134, 90, 'comp_soft_dev');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(144, 144, 'Системное программирование', 'System Programming', 134, 100, 'comp_soft_dev_system');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(145, 145, 'Экспертные системы', 'Expert Systems', 134, 110, 'comp_db_exp');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(146, 146, 'Компьютеры: Языки и системы программирования', 'Programming Languages', 0, null, null);
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(147, 147, '.NET Framework', '.NET Framework', 146, 10, 'comp_prog_dotnet');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(148, 148, 'Assembler', 'Assembler', 146, 20, 'comp_prog_assembler');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(149, 149, 'Basic, Visual Basic, VB Script, VBA и т.п.', 'Basic, Visual Basic, VB Script, VBA', 146, 30, 'comp_prog_basic');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(150, 150, 'C, C++, C#', 'C, C++, C#', 146, 40, 'comp_prog_c');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(151, 151, 'Forth', 'Forth', 146, 50, 'comp_prog_forth');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(152, 152, 'Java, Java Script', 'Java, Java Script', 146, 60, 'comp_prog_java');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(153, 153, 'Lisp', 'Lisp', 146, 70, 'comp_prog_lisp');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(154, 154, 'Lua', 'Lua', 146, 80, 'comp_prog_lua');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(155, 155, 'MFC', 'MFC', 146, 90, 'comp_prog_mfc');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(156, 156, 'Pascal, Delphi, Lazarus и т.п.', 'Pascal, Delphi, Lazarus', 146, 100, 'comp_prog_pascal');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(157, 157, 'PHP', 'PHP', 146, 110, 'comp_prog_php');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(158, 158, 'Prolog', 'Prolog', 146, 120, 'comp_prog_prolog');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(159, 159, 'Python', 'Python', 146, 130, 'comp_prog_python');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(160, 160, 'Qt', 'Qt', 146, 140, 'comp_prog_qt');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(161, 161, 'Ruby', 'Ruby', 146, 150, 'comp_prog_ror');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(162, 162, 'Windows API', 'Windows API', 146, 160, 'comp_prog_winapi');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(163, 163, 'Другие языки и системы программирования', 'Other Programming Languages', 146, 170, 'comp_programming');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(164, 164, 'Культура и искусство', 'Art, Art History, Design', 0, null, null);
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(165, 165, 'Архитектура и скульптура', 'Architecture', 164, 10, 'architecture_book');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(166, 166, 'Живопись, альбомы, иллюстрированные каталоги', 'Painting', 164, 20, 'painting');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(167, 167, 'Изобразительное искусство, фотография', 'Visual Arts', 164, 30, 'visual_arts');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(168, 168, 'Искусство и дизайн', 'Art, Design', 164, 40, 'design');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(169, 169, 'Искусствоведение', 'Art Criticism', 164, 50, 'art_criticism');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(170, 170, 'История искусства', 'Art History', 164, 60, 'art_history');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(171, 171, 'Кино', 'Cine', 164, 70, 'cine');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(172, 172, 'Критика', 'Criticism', 164, 80, 'nonf_criticism');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(173, 173, 'Культурология', 'Cultural Science', 164, 90, 'sci_culture');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(174, 174, 'Мировая художественная культура', 'Art World Culture', 164, 100, 'art_world_culture');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(175, 175, 'Мода и стиль', 'Fashion Style', 164, 110, 'fashion_style');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(176, 176, 'Музыка', 'Music', 164, 120, 'music');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(177, 177, 'Партитуры', 'Notes', 164, 130, 'notes');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(178, 178, 'Радио и телевидение', 'Radio and TV', 164, 140, 'radio_tv');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(179, 179, 'Танцы и хореография', 'Dance and Choreography', 164, 150, 'art_dance');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(180, 180, 'Театр', 'Theatre', 164, 160, 'theatre');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(181, 181, 'Литература по изданиям', 'Publications', 0, null, null);
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(182, 182, 'Дореволюционные зарубежные издания', 'Pre-revolutionary Foreign Publications', 181, 10, 'old_foreign_publication');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(183, 183, 'Дореволюционные российские издания', 'Pre-revolutionary Russian Publications', 181, 20, 'old_rus_publication');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(184, 184, 'Зарубежные издания советского периода', 'Foreign publications of the Soviet period', 181, 30, 'foreign_su_publication');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(185, 185, 'Раритетные издания', 'Rare Editions', 181, 40, 'rarity');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(186, 186, 'Самиздат, сетевая литература', 'Network Literature', 181, 50, 'network_literature');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(187, 187, 'Советские издания', 'Soviet Publications', 181, 60, 'su_publication');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(188, 188, 'Современные зарубежные издания', 'Modern Foreign Publications', 181, 70, 'foreign_publication');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(189, 189, 'Современные издания стран бывшего СССР', 'Publications Of The Former USSR', 181, 80, 'ex_su_publication');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(190, 190, 'Современные российские издания', 'Modern Russian Publications', 181, 90, 'rus_publication');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(191, 191, 'Литература по эпохам', 'Literature Ages', 0, null, null);
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(192, 192, 'Литература IV века и ранее (эпоха Древнего мира)', 'Literature Before The V Century', 191, 10, 'literature_4');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(193, 193, 'Литература V-XIII веков (эпоха Средневековья)', 'Literature Of V-XIII Centuries', 191, 20, 'foreign_antique');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(194, 194, 'Литература XIV-XVI веков (эпоха Возрождения)', 'Literature Of XIV-XVI Centuries', 191, 30, 'literature_16');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(195, 195, 'Литература XVII-XVIII веков (эпоха Просвящения)', 'Literature Of XVII-XVIII Centuries', 191, 40, 'literature_18');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(196, 196, 'Литература ХIX века (эпоха Промышленной революции)', 'Literature Of The XIX Century', 191, 50, 'literature_19');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(197, 197, 'Литература ХX века (эпоха Социальных революций)', 'Literature Of The ХX Century', 191, 60, 'literature_20');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(198, 198, 'Литература ХXI века (эпоха Глобализации экономики)', 'Literature Of The ХXI Century', 191, 70, 'literature_21');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(199, 199, 'Любовные романы', 'Romance', 0, null, null);
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(200, 200, 'Исторические любовные романы', 'Historical Romance', 199, 10, 'love_history');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(201, 201, 'Короткие любовные романы', 'Short Romance', 199, 20, 'love_short');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(202, 202, 'Любовная фантастика', 'Love SF', 199, 30, 'love_sf');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(203, 203, 'Любовное фэнтези', 'Love Fantasy', 199, 40, 'love_fantasy');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(204, 204, 'Любовные детективы', 'Detective Romance', 199, 50, 'love_detective');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(205, 205, 'О любви', 'Love Romance', 199, 60, 'love');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(206, 206, 'Порно', 'Porno', 199, 70, 'love_hard');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(207, 207, 'Сексуальные извращения', 'Sexual Perversion', 199, 80, 'sexual_perversion');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(208, 208, 'Слэш', 'Love Slash', 199, 90, 'love_slash');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(209, 209, 'Современные любовные романы', 'Contemporary Romance', 199, 100, 'love_contemporary');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(210, 210, 'Фемслеш', 'Love Femslash', 199, 110, 'love_femslash');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(211, 211, 'Эротика', 'Erotica', 199, 120, 'love_erotica');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(212, 212, 'Наука, образование', 'Scientific-educational', 0, null, null);
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(213, 213, 'Альтернативная медицина', 'Alternative Medicine', 212, 10, 'sci_medicine_alternative');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(214, 214, 'Альтернативные науки и научные теории', 'Theories', 212, 20, 'sci_theories');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(215, 215, 'Аналитическая химия', 'Analitic Chemistry', 212, 30, 'sci_anachem');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(216, 216, 'Астрономия и Космос', 'Cosmos', 212, 40, 'sci_cosmos');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(217, 217, 'Ветеринария', 'Veterinary', 212, 50, 'sci_veterinary');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(218, 218, 'Востоковедение', 'Science Oriental', 212, 60, 'sci_oriental');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(219, 219, 'География', 'Geography', 212, 70, 'sci_geography');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(220, 220, 'Геология и геофизика', 'Geology and Geophysics', 212, 80, 'sci_geo');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(221, 221, 'Государство и право', 'State Science', 212, 90, 'sci_state');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(222, 222, 'Иностранные языки', 'Foreign Languages', 212, 100, 'foreign_language');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(223, 223, 'История науки', 'Science History', 212, 110, 'science_history');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(224, 224, 'Краеведение', 'Study of Local Lore', 212, 120, 'local_lore_study');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(225, 225, 'Литературоведение', 'Philology', 212, 130, 'sci_philology');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(226, 226, 'Математика', 'Mathematics', 212, 140, 'sci_math');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(227, 227, 'Медицина', 'Medicine', 212, 150, 'sci_medicine');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(228, 228, 'Научная литература', 'Science Other', 212, 160, 'science');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(229, 229, 'Обществознание', 'Social Studies', 212, 170, 'sci_social_studies');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(230, 230, 'Органическая химия', 'Органическая химия', 212, 180, 'sci_orgchem');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(231, 231, 'Педагогика', 'Pedagogy', 212, 190, 'sci_pedagogy');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(232, 232, 'Политика и дипломатия', 'Politics and Diplomacy', 212, 200, 'sci_politics');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(233, 233, 'Религиоведение', 'Religious Studies', 212, 210, 'sci_religion');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(234, 234, 'Физика', 'Physics', 212, 220, 'sci_phys');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(235, 235, 'Физическая химия', 'Physical Chemistry', 212, 230, 'sci_physchem');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(236, 236, 'Философия', 'Philosophy', 212, 240, 'sci_philosophy');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(237, 237, 'Химия', 'Chemistry', 212, 250, 'sci_chem');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(238, 238, 'Экономика', 'Economic', 212, 260, 'sci_economy');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(239, 239, 'Юриспруденция', 'Jurisprudence', 212, 270, 'sci_juris');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(240, 240, 'Языкознание', 'Linguistics', 212, 280, 'sci_linguistic');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(241, 241, 'Науки о живой природе', 'Biology', 0, null, null);
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(242, 242, 'Биология', 'Biology', 241, 10, 'sci_biology');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(243, 243, 'Биофизика', 'Biophysics', 241, 20, 'sci_biophys');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(244, 244, 'Биохимия', 'Biochemistry', 241, 30, 'sci_biochem');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(245, 245, 'Ботаника', 'Botany', 241, 40, 'sci_botany');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(246, 246, 'Зоология', 'Zoology', 241, 50, 'sci_zoo');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(247, 247, 'Эволюционизм', 'Evolutionism', 241, 60, 'sci_evolutionism');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(248, 248, 'Экология', 'Ecology', 241, 70, 'sci_ecology');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(249, 249, 'Поэзия', 'Poetry', 0, null, null);
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(250, 250, 'Басни', 'Fable', 249, 10, 'fable');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(251, 251, 'В стихах', 'In Verse', 249, 20, 'in_verse');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(252, 252, 'Верлибры', 'Vers Libre', 249, 30, 'vers_libre');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(253, 253, 'Визуальная поэзия', 'Visual Poetry', 249, 40, 'visual_poetry');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(254, 254, 'Классическая зарубежная поэзия', 'Classical Foreign Poetry', 249, 50, 'poetry_for_classical');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(255, 255, 'Классическая поэзия', 'Classical Poetry', 249, 60, 'poetry_classical');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(256, 256, 'Классическая русская поэзия', 'Classical Russian Poetry', 249, 70, 'poetry_rus_classical');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(257, 257, 'Лирика', 'Lyrics', 249, 80, 'lyrics');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(258, 258, 'Палиндромы', 'Palindromes', 249, 90, 'palindromes');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(259, 259, 'Песенная поэзия', 'Song Poetry', 249, 100, 'song_poetry');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(260, 260, 'Поэзия', 'Poetry', 249, 110, 'poetry');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(261, 261, 'Поэзия Востока', 'Poetry Of The East', 249, 120, 'poetry_east');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(262, 262, 'Поэма', 'Poem', 249, 130, 'poem');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(263, 263, 'Советская поэзия', 'Soviet Poetry', 249, 140, 'poetry_su');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(264, 264, 'Современная зарубежная поэзия', 'Modern Foreign Poetry', 249, 150, 'poetry_for_modern');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(265, 265, 'Современная поэзия', 'Modern Poetry', 249, 160, 'poetry_modern');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(266, 266, 'Современная русская поэзия', 'Modern Russian Poetry', 249, 170, 'poetry_rus_modern');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(267, 267, 'Стихи о войне', 'Military Poetry', 249, 180, 'poetry_military');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(268, 268, 'Экспериментальная поэзия', 'Experimental Poetry', 249, 190, 'experimental_poetry');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(269, 269, 'Эпическая поэзия', 'Epic Poetry', 249, 200, 'epic_poetry');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(270, 270, 'Приключения', 'Adventure', 0, null, null);
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(271, 271, 'Авантюрный роман', 'Adventure story', 270, 10, 'adv_story');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(272, 272, 'Вестерны', 'Western', 270, 20, 'adv_western');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(273, 273, 'Военные приключения', 'Military Adventure', 270, 30, 'adv_military');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(274, 274, 'Исторические приключения', 'History Fiction', 270, 40, 'adv_history');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(275, 275, 'Морские приключения', 'Maritime Fiction', 270, 50, 'adv_maritime');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(276, 276, 'Приключения в современном мире', 'Adventure Modern', 270, 60, 'adv_modern');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(277, 277, 'Приключения про индейцев', 'Indians', 270, 70, 'adv_indian');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(278, 278, 'Приключения', 'Adventure Other', 270, 80, 'adventure');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(279, 279, 'Природа и животные', 'Nature & Animals', 270, 90, 'adv_animal');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(280, 280, 'Путешествия и география', 'Travel & Geography', 270, 100, 'adv_geo');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(281, 281, 'Рыцарский роман', 'Tale Chivalry', 270, 110, 'tale_chivalry');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(282, 282, 'Советская приключенческая литература', 'Soviet Adventure', 270, 120, 'adv_su');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(283, 283, 'Проза', 'Prose', 0, null, null);
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(284, 284, 'Афоризмы и цитаты', 'Aphorisms', 283, 10, 'aphorisms');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(285, 285, 'В прозе', 'In Prose', 283, 20, 'in_prose');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(286, 286, 'Военная проза', 'Military Prose', 283, 30, 'prose_military');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(287, 287, 'Зарубежная классическая проза', 'Foreign Classical Prose', 283, 40, 'foreign_prose');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(288, 288, 'Зарубежная современная проза', 'Foreign Contemporary Prose', 283, 50, 'foreign_contemporary');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(289, 289, 'Историческая проза', 'Historical Prose', 283, 60, 'prose_history');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(290, 290, 'Классическая проза', 'Classics Prose', 283, 70, 'prose_classic');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(291, 291, 'Контркультура', 'Counterculture', 283, 80, 'prose_counter');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(292, 292, 'Магический реализм', 'Magic Realism', 283, 90, 'prose_magic');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(293, 293, 'Новелла', 'Story', 283, 100, 'story');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(294, 294, 'Повесть', 'Great Story', 283, 110, 'great_story');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(295, 295, 'Проза', 'Prose', 283, 120, 'prose');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(296, 296, 'Рассказ', 'Short Story', 283, 130, 'short_story');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(297, 297, 'Роман', 'Roman', 283, 140, 'roman');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(298, 298, 'Русская классическая проза', 'Russian Classics', 283, 150, 'prose_rus_classic');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(299, 299, 'Русская современная проза', 'Russian Contemporary Prose', 283, 160, 'russian_contemporary');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(300, 300, 'Семейная сага', 'Saga', 283, 170, 'sagas');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(301, 301, 'Сентиментальная проза', 'Sentimental Prose', 283, 180, 'prose_sentimental');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(302, 302, 'Советская проза', 'Soviet Classics', 283, 190, 'prose_su_classics');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(303, 303, 'Современная проза', 'Contemporary Prose', 283, 200, 'prose_contemporary');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(304, 304, 'Фантасмагория, абсурдистская проза', 'Phantasmagoria, Absurdist Prose', 283, 210, 'prose_abs');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(305, 305, 'Феерия', 'Extravaganza', 283, 220, 'extravaganza');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(306, 306, 'Экспериментальная, неформатная проза', 'Experimental, Informal Prose', 283, 230, 'prose_neformatny');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(307, 307, 'Эпистолярная проза', 'Epistolary Fiction', 283, 240, 'epistolary_fiction');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(308, 308, 'Эпопея', 'Epic', 283, 250, 'prose_epic');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(309, 309, 'Эссе, очерк, этюд, набросок', 'Essay', 283, 260, 'essay');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(310, 310, 'Прочее', 'Other', 0, null, null);
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(311, 311, 'Авторские сборники, собрания сочинений', 'Autor Collections', 310, 10, 'autor_collection');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(312, 312, 'Антисоветская литература', 'Anti-Soviet Literature', 310, 20, 'dissident');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(313, 313, 'Бестселлеры', 'Bestsellers', 310, 30, 'bestseller');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(314, 314, 'В пересказе, в лит. обработке', 'In retelling', 310, 40, 'in_retelling');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(315, 315, 'В сокращении', 'In reduction', 310, 50, 'in_reduction');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(316, 316, 'Газеты и журналы', 'Periodic', 310, 60, 'periodic');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(317, 317, 'Диафильмы', 'Diafilm', 310, 70, 'diafilm');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(318, 318, 'Для взрослых', 'For Adults', 310, 80, 'adult');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(319, 319, 'Книга-игра', 'Book-game', 310, 90, 'prose_game');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(320, 320, 'Комиксы', 'Comics', 310, 100, 'comics');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(321, 321, 'Компиляции', 'Compilation', 310, 110, 'compilation');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(322, 322, 'Кошки', 'Cats', 310, 120, 'cats');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(323, 323, 'Литературные сказки', 'Literary Fairy Tales', 310, 130, 'literary_fairy_tale');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(324, 324, 'Любительские переводы', 'Fan Translations', 310, 140, 'fan_translation');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(325, 325, 'Наборы открыток', 'Postcards', 310, 150, 'postcards');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(326, 326, 'Начинающие авторы', 'Beginning Authors', 310, 160, 'beginning_authors');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(327, 327, 'Недописанное', 'Unfinished', 310, 170, 'unfinished');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(328, 328, 'Неотсортированное', 'Not Sorting', 310, 180, 'other');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(329, 329, 'Новеллизации', 'Novelization', 310, 190, 'novelization');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(330, 330, 'Отрывок, ознакомительный фрагмент', 'Fragment', 310, 200, 'fragment');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(331, 331, 'Сборники, альманахи, антологии', 'Collections', 310, 210, 'collection');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(332, 332, 'Фанфики', 'Fan Fictions', 310, 220, 'fanfiction');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(333, 333, 'Психология и психиатрия', 'Psychology and Psychiatry', 0, null, null);
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(334, 334, 'Гипноз, внушение и самовнушение', 'Hypnosis', 333, 10, 'sci_hypnosis');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(335, 335, 'Детская психология', 'Child Psychology', 333, 20, 'psy_childs');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(336, 336, 'Психиатрия и наркология', 'Psychiatry and Narcology', 333, 30, 'sci_psychiatry');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(337, 337, 'Психология', 'Psychology', 333, 40, 'sci_psychology');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(338, 338, 'Психотерапия и консультирование', 'Psychotherapy', 333, 50, 'psy_theraphy');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(339, 339, 'Секс и семейная психология', 'Sex and Family', 333, 60, 'psy_sex_and_family');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(340, 340, 'Религия и духовность', 'Religion & Inspiration', 0, null, null);
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(341, 341, 'Астрология', 'Astrology', 340, 10, 'astrology');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(342, 342, 'Буддизм', 'Buddhism', 340, 20, 'religion_budda');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(343, 343, 'Индуизм', 'Hinduism', 340, 30, 'religion_hinduism');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(344, 344, 'Ислам', 'Islam', 340, 40, 'religion_islam');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(345, 345, 'Иудаизм', 'Judaism', 340, 50, 'religion_judaism');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(346, 346, 'Католицизм', 'Catholicism', 340, 60, 'religion_catholicism');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(347, 347, 'Православие', 'Orthodoxy', 340, 70, 'religion_orthodoxy');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(348, 348, 'Протестантизм', 'Protestantism', 340, 80, 'religion_protestantism');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(349, 349, 'Религиозная литература', 'Religion Other', 340, 90, 'religion');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(350, 350, 'Религия', 'Religion', 340, 100, 'religion_rel');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(351, 351, 'Самосовершенствование', 'Self-improvement', 340, 110, 'religion_self');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(352, 352, 'Хиромантия', 'Palmistry', 340, 120, 'palmistry');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(353, 353, 'Христианство', 'Christianity', 340, 130, 'religion_christianity');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(354, 354, 'Эзотерика', 'Esoterics', 340, 140, 'religion_esoterics');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(355, 355, 'Язычество', 'Paganism', 340, 150, 'religion_paganism');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(356, 356, 'Справочная литература', 'Reference', 0, null, null);
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(357, 357, 'Путеводители', 'Guides', 356, 10, 'geo_guides');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(358, 358, 'Руководства', 'Guidebooks', 356, 20, 'ref_guide');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(359, 359, 'Самоучители', 'Self-tutors', 356, 30, 'ref_self_tutor');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(360, 360, 'Словари', 'Dictionaries', 356, 40, 'ref_dict');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(361, 361, 'Справочная литература', 'Reference Other', 356, 50, 'reference');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(362, 362, 'Справочники', 'Reference', 356, 60, 'ref_ref');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(363, 363, 'Энциклопедии', 'Encyclopedias', 356, 70, 'ref_encyc');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(364, 364, 'Старинное', 'Antique Literature', 0, null, null);
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(365, 365, 'Античная литература', 'Antique', 364, 10, 'antique_ant');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(366, 366, 'Древневосточная литература', 'Old East', 364, 20, 'antique_east');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(367, 367, 'Древнерусская литература', 'Old Russian', 364, 30, 'antique_russian');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(368, 368, 'Древнеевропейская литература', 'European', 364, 40, 'antique_european');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(369, 369, 'Старинная литература', 'Other', 364, 50, 'antique');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(370, 370, 'Техника', 'Technics', 0, null, null);
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(371, 371, 'Автодело', 'Auto Business', 370, 10, 'auto_business');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(372, 372, 'История техники', 'Equ History', 370, 20, 'equ_history');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(373, 373, 'Конструирование', 'Engineering', 370, 30, 'sci_engineering');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(374, 374, 'Материаловедение, конструкционные материалы', 'Materials', 370, 40, 'sci_tech_materials');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(375, 375, 'Машиностроение', 'Machinery', 370, 50, 'sci_tech_machinery');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(376, 376, 'Металлургия', 'Metallurgy', 370, 60, 'sci_metal');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(377, 377, 'Нормативная техническая документация', 'Tech Standards', 370, 70, 'sci_tech_standards');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(378, 378, 'Радиоэлектроника', 'Radio', 370, 80, 'sci_radio');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(379, 379, 'Ракетостроение и космическая техника', 'Rockets', 370, 90, 'sci_tech_rockets');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(380, 380, 'Строительная механика и сопромат', 'Strength Of Materials', 370, 100, 'sci_tech_sopromat');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(381, 381, 'Строительство', 'Building', 370, 110, 'sci_build');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(382, 382, 'Технические науки', 'Technical', 370, 120, 'sci_tech');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(383, 383, 'Транспорт и авиация', 'Transport', 370, 130, 'sci_transport');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(384, 384, 'Учебники и пособия', 'Textbook', 0, null, null);
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(385, 385, 'Рефераты', 'Abstract', 384, 10, 'sci_abstract');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(386, 386, 'Советские учебники и пособия', 'Soviet Textbook', 384, 20, 'sci_textbook_su');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(387, 387, 'Учебники и пособия', 'Textbook', 384, 30, 'sci_textbook');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(388, 388, 'Учебники и пособия ВУЗов', 'Textbook Higher', 384, 40, 'tbg_higher');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(389, 389, 'Учебники и пособия для среднего и специального образования', 'Textbook Secondary', 384, 50, 'tbg_secondary');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(390, 390, 'Школьные учебники и пособия', 'Textbook School', 384, 60, 'tbg_school');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(391, 391, 'Шпаргалки', 'Cribs', 384, 70, 'sci_crib');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(392, 392, 'Фантастика', 'Science Fiction', 0, null, null);
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(393, 393, 'Альтернативная история', 'Alternative History', 392, 10, 'sf_history');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(394, 394, 'Боевая фантастика', 'Action Science Fiction', 392, 20, 'sf_action');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(395, 395, 'Героическая фантастика', 'Heroic Science Fiction', 392, 30, 'sf_heroic');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(396, 396, 'Детективная фантастика', 'Detective Science Fiction', 392, 40, 'sf_detective');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(397, 397, 'Зарубежная фантастика', 'Foreign Science Fiction', 392, 50, 'foreign_sf');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(398, 398, 'Ироническая фантастика', 'Ironical Science Fiction', 392, 60, 'sf_irony');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(399, 399, 'Киберпанк', 'Cyberpunk', 392, 70, 'sf_cyberpunk');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(400, 400, 'Космическая опера', 'Space Opera', 392, 80, 'sf_space_opera');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(401, 401, 'Космическая фантастика', 'Space Science Fiction', 392, 90, 'sf_space');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(402, 402, 'ЛитРПГ', 'LitRPG', 392, 100, 'sf_litrpg');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(403, 403, 'Мистика', 'Mystic', 392, 110, 'sf_mystic');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(404, 404, 'Научная фантастика', 'Science Fiction', 392, 120, 'sf');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(405, 405, 'Ненаучная фантастика', 'Non Science Fiction', 392, 130, 'nsf');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(406, 406, 'Палеонтологическая фантастика', 'Paleontological Science Fiction', 392, 140, 'sf_paleontological');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(407, 407, 'Попаданцы', 'Popadanec', 392, 150, 'popadanec');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(408, 408, 'Постапокалипсис', 'Postapocalyptic', 392, 160, 'sf_postapocalyptic');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(409, 409, 'Российская фантастика', 'Russian Science Fiction', 392, 170, 'sf_rus');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(410, 410, 'Советская фантастика', 'Soviet Science Fiction', 392, 180, 'sf_su');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(411, 411, 'Социально-философская фантастика', 'Social-philosophical', 392, 190, 'sf_social');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(412, 412, 'Стимпанк', 'Stimpank', 392, 200, 'sf_stimpank');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(413, 413, 'Ужасы', 'Horror', 392, 210, 'sf_horror');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(414, 414, 'Фантастика', 'Other Science Fiction', 392, 220, 'sf_etc');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(415, 415, 'Хроноопера', 'Hronoopera', 392, 230, 'hronoopera');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(416, 416, 'Эпическая фантастика', 'Epic Science Fiction', 392, 240, 'sf_epic');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(417, 417, 'Юмористическая фантастика', 'Humor Science Fiction', 392, 250, 'sf_humor');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(418, 418, 'Фольклор', 'Folklore', 0, null, null);
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(419, 419, 'Былины', 'Epic', 418, 10, 'epic');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(420, 420, 'Загадки', 'Riddles', 418, 20, 'riddles');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(421, 421, 'Мифы, легенды, эпос', 'Myths, Legends, Epos', 418, 30, 'antique_myths');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(422, 422, 'Народные песни', 'Folk Songs', 418, 40, 'folk_songs');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(423, 423, 'Народные приметы, обряды, традиции', 'Folk Traditions', 418, 50, 'folk_traditions');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(424, 424, 'Народные сказки', 'Folk Tales', 418, 60, 'folk_tale');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(425, 425, 'Пословицы и поговорки', 'Proverbs', 418, 70, 'proverbs');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(426, 426, 'Русский фольклор', 'Russian Folklore', 418, 80, 'folklore_rus');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(427, 427, 'Фольклор: прочее', 'Folklore', 418, 90, 'folklore');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(428, 428, 'Частушки, прибаутки, потешки', 'Limerick', 418, 100, 'limerick');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(429, 429, 'Фэнтези', 'Fantasy', 0, null, null);
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(430, 430, 'Боевое фэнтези', 'Fight Fantasy', 429, 10, 'fantasy_fight');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(431, 431, 'Городское фэнтези', 'Fantasy City', 429, 20, 'sf_fantasy_city');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(432, 432, 'Готический роман', 'Gothic Novel', 429, 30, 'gothic_novel');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(433, 433, 'Зарубежное фэнтези', 'Foreign Fantasy', 429, 40, 'foreign_fantasy');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(434, 434, 'Ироническое фэнтези', 'Ironyc Female Fantasy', 429, 50, 'sf_fantasy_irony');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(435, 435, 'Историческое фэнтези', 'Historical Fantasy', 429, 60, 'historical_fantasy');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(436, 436, 'Магическое фэнтези', 'Magical Fantasy', 429, 70, 'magician_book');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(437, 437, 'О вампирах', 'Vampire Fantasy', 429, 80, 'vampire_book');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(438, 438, 'О драконах', 'Dragon Fantasy', 429, 90, 'dragon_fantasy');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(439, 439, 'Сказочная фантастика', 'Fairy Fantasy', 429, 100, 'fairy_fantasy');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(440, 440, 'Славянское фэнтези', 'Slavic Fantasy', 429, 110, 'russian_fantasy');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(441, 441, 'Современная сказка', 'Modern Tale', 429, 120, 'modern_tale');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(442, 442, 'Технофэнтези', 'Technofantasy', 429, 130, 'sf_technofantasy');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(443, 443, 'Фэнтези: прочее', 'Fantasy', 429, 140, 'sf_fantasy');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(444, 444, 'Юмористическое фэнтези', 'Humor Fantasy', 429, 150, 'humor_fantasy');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(445, 445, 'Хобби и ремесла', 'Hobbies & Crafts', 0, null, null);
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(446, 446, 'Авто- и мототранспорт, ПДД', 'Auto Regulations', 445, 10, 'auto_regulations');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(447, 447, 'Аквариумистика', 'Aquarium', 445, 20, 'home_aquarium');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(448, 448, 'Альпинизм и скалолазание', 'Mountaineering', 445, 30, 'home_mountain');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(449, 449, 'Виноделие, спиртные напитки', 'Winemaking', 445, 40, 'home_winemaking');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(450, 450, 'Животноводство и птицеводство', 'Livestock Farming', 445, 50, 'home_livestock');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(451, 451, 'Изготовление и ремонт мебели', 'Furniture', 445, 60, 'home_furniture');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(452, 452, 'Инвентарь, инструменты', 'Inventory, Instruments', 445, 70, 'home_inventory');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(453, 453, 'Индивидуальное строительство и ремонт', 'Building and Renovation', 445, 80, 'home_building');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(454, 454, 'Книгоделие', 'Bookmaking', 445, 90, 'home_bookmaking');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(455, 455, 'Коллекционирование', 'Collecting', 445, 100, 'home_collecting');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(456, 456, 'Морское дело, парусный спорт', 'Maritime Affairs', 445, 110, 'home_marine');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(457, 457, 'Охота и охотоведение', 'Hunt', 445, 120, 'home_hunt');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(458, 458, 'Писательское искусство', 'Writing Art', 445, 130, 'home_writing_art');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(459, 459, 'Пчеловодство', 'Beekeeping', 445, 140, 'home_beekeeping');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(460, 460, 'Работа по дереву', 'Woodwork', 445, 150, 'home_woodwork');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(461, 461, 'Работа по металлу', 'Metal Work', 445, 160, 'home_metalwork');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(462, 462, 'Рукоделие', 'Handiwork', 445, 170, 'home_handiwork');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(463, 463, 'Рыболовство и рыбоводство', 'Fishing', 445, 180, 'home_fishing');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(464, 464, 'Сад и огород', 'Garden', 445, 190, 'home_garden');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(465, 465, 'Сбор и выращивание грибов', 'Mushrooms', 445, 200, 'home_mushrooms');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(466, 466, 'Сделай сам', 'Do It Yourself', 445, 210, 'home_diy');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(467, 467, 'Спорт', 'Sports', 445, 220, 'home_sport');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(468, 468, 'Туризм', 'Tourism', 445, 230, 'home_tourism');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(469, 469, 'Хобби и ремесла: прочее', 'Hobbies & Crafts Other', 445, 240, 'home_crafts');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(470, 470, 'Цветоводство', 'Floriculture', 445, 250, 'home_floriculture');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(471, 471, 'Юмор', 'Humor', 0, null, null);
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(472, 472, 'Анекдоты', 'Anecdote', 471, 10, 'humor_anecdote');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(473, 473, 'Байки', 'Tales', 471, 20, 'humor_tales');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(474, 474, 'Пародии, шаржи, эпиграммы', 'Parodies, Cartoons, Epigrams', 471, 30, 'humor_parody');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(475, 475, 'Сатира', 'Satire', 471, 40, 'humor_satire');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(476, 476, 'Юмор', 'Humor Other', 471, 50, 'humor');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(477, 477, 'Юмористическая проза', 'Humor Prose', 471, 60, 'humor_prose');
INSERT INTO [genre]([rowid], [id], [name], [name_en], [id_parent], [sort_index], [code]) VALUES(478, 478, 'Юмористические стихи', 'Humor Verses', 471, 70, 'humor_verse');


/* Table data [params] Record count: 1 */
INSERT INTO [params]([rowid], [id], [name], [value]) VALUES(1, 1, 'version', '7');

/* Commit transaction */
COMMIT;

/* Enable foreign keys */
PRAGMA foreign_keys = 'on';
