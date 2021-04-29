# usoSQL

usoSQL is a database program in C that implements basic SQL commands to store and manipulate data.

## Table of contents
<!--ts-->
   * [Usage](#usage)
   * [Contributing](#contributing)
   * [Contact](#contact)
<!--te-->

## Usage 
**Open the database:**
```
./main name_db
```

**Create a table:**
```
CREATE TABLE table_name
column1 int
column2 char(255)
column3 char(255)
column4 char(255)
column5 char(255)
.
.
.
columnN char(255);
```

**Insert data into a table:**

```
INSERT INTO table_name
value1 value2 value3 ...;
```

**Update data from a table:**
```
UPDATE table_name
SET column1 = value1 column2 = value2 ...
WHERE condition;
```
**Select data from a table:**
```
SELECT column1 column2 ...
FROM table_name;
```
**Delete data from a table:**
```
DELETE FROM table_name WHERE condition;
```
**Delete database:**
```
DROP DATABASE name_db
```

## Contributing
Pull requests are welcome. For major changes, please open an issue first to discuss what you would like to change.


## Contact
* **Moraru Radu-Andrei** (radu.a.moraru@gmail.com)
* **Matei Tudor-Cristian** (cristianmateix2001@gmail.com)
* **Trăncănău Cezar-Alexandru** (cezartrancanau@gmail.com)
* **Brinceanu Andi-Mădălin** (andi.madalin@gmail.com)
