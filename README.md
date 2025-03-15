# Movie Manager System

## Description
This project is a **Movie Management System** implemented in **C**, utilizing **indexed files** for data storage and retrieval. The system allows users to add, modify, and list movies efficiently using **primary and secondary indexing**.

## Features
- **Add movies** to the database
- **Modify movie ratings** by key
- **List movies** by key or title
- **View all movies** in the database
- **Uses indexed files** for efficient searching and storage

## File Structure
- `main.c` - The main program file containing the movie management system logic.
- `movies.c` - Contains functions for handling movie operations (add, modify, list, etc.).
- `movies.dat` - The primary movie database file.
- `ibtree.idx` - The **B-tree index file** for movie records.
- `ititle.idx` - The **secondary index file** for searching movies by title.

## Compilation and Execution
To compile the project, use:

```sh
gcc main.c movies.c -o movie_manager
```

To execute the program:

```sh
./movie_manager
```

## Usage
After running the program, a menu will be displayed:

```
|-------------------------------|
|   Movie Management System     |
|-------------------------------|
| 1 - Add Movie                 |
| 2 - Modify Rating by Key      |
| 3 - List Movie by Key         |
| 4 - List Movie by Title       |
| 5 - List All Movies           |
|-------------------------------|
Choose an option (1-5) (* - quit):
```

Select the desired operation by entering the corresponding number.

## Indexing System
- The **B-tree index (`ibtree.idx`)** maintains the organization of movie records.
- The **secondary index (`ititle.idx`)** ensures efficient searches by title.
- If inconsistencies are detected, the secondary index is **reconstructed automatically**.

## Notes
- If the movie database (`movies.dat`) or index files do not exist, they will be created upon the first execution.
- Ensure that the `movies.c` file contains all necessary functions for handling movie operations.

## Future Improvements
- Implement **file compression** for storage optimization.
- Add **movie deletion functionality**.
- Improve **error handling and logging**.

---
**Author**: Your Name  
**Language**: C  
**License**: MIT  

