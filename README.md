# usoSQL

**usoSQL** is a database program in **C** that implements basic **SQL** commands to store and manipulate data.

## Table of contents
<!--ts-->
   * [Usage](#usage)
   * [Contributing](#contributing)
   * [Contact](#contact)
<!--te-->

## Usage 
**Open database:**
```bash
./main name_db
```

**Create a table** **(data_type** can be either **int**, **char(nr)** or **float):**
```sql
CREATE TABLE table_name
column1_name text data_type
column2_name text data_type
column3_name text data_type
column4_name text data_type
column5_name text data_type
.
.
.
columnN_name text data_type;
```

**Insert data into a table:**

```sql
INSERT INTO table_name
value1 value2 value3 ...;
```

**Update data from a table:**
```sql
UPDATE table_name
SET column1_name = value1 column2_name = value2 ... 
WHERE condition;
```
**Select data from a table:**
```sql
SELECT column1_name column2_name ...
FROM table_name;
```
**Delete data from a table:**
```sql
DELETE FROM table_name WHERE condition;
```
**Delete database:**
```sql
DROP DATABASE name_db
```

## Contributing
Pull requests are welcome. For major changes, please open an issue first to discuss what you would like to change.


## Contact
* **Moraru Radu-Andrei** (radu.a.moraru@gmail.com)
* **Matei Tudor-Cristian** (cristianmateix2001@gmail.com)
* **Trăncănău Cezar-Alexandru** (cezartrancanau@gmail.com)
* **Brînceanu Andi-Mădălin** (andi.madalin@gmail.com)
