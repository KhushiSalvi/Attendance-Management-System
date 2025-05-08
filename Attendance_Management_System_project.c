#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

// Structure Definitions
typedef struct {
    char name[50];        // Increased size for student names
    char rollno[15];      // Accommodates longer roll numbers
    char att[4][31];      // Attendance for 4 months and 31 days
} student;

typedef struct {
    char course_id[15];    // Course ID
    char course_name[100]; // Course Name
} course;

// Function Declarations
void ADDcourse(FILE *dtb, int nc);
void VIEWcourses(FILE *dtb, int nc);
void ADDstudent(FILE *coursefile);
void MARKattendance(FILE *coursefile);
void EDITattendance(FILE *coursefile);
void VIEWattendance(FILE *coursefile);
void CREATErecord(FILE *coursefile);
void VIEWhistory();
int USERstatus(); // Returns: 1 for students, 2 for teachers, 3 for admins

int main() {
    // Initialize database and course count
    int nc = 0;
    FILE *dtb = fopen("dtb.bin", "rb+");
    if (dtb == NULL) {
        dtb = fopen("dtb.bin", "wb+"); // Create database if it doesn't exist
        if (dtb == NULL) {
            perror("Error creating database file");
            return EXIT_FAILURE;
        }
        fwrite(&nc, sizeof(int), 1, dtb); // Initialize course count
    }
    rewind(dtb);
    fread(&nc, sizeof(int), 1, dtb);

    // Check User's Status
    int access = USERstatus();

    // Access-Based Menu System
    while (access) {
        switch (access) {
            case 1: // Student Access
                while (1) {
                    printf("\n--- STUDENT MENU ---\n");
                    printf("1. View Attendance\n2. Change Access Permissions\n3. Exit\n");
                    printf("Enter your choice: ");
                    int choice;
                    scanf("%d", &choice);
                    while (getchar() != '\n'); // Clear input buffer

                    if (choice == 1) {
                        char course_file[20];
                        printf("Enter Course ID: ");
                        scanf("%15s", course_file);
                        strcat(course_file, ".bin");
                        FILE *coursefile = fopen(course_file, "rb");
                        if (coursefile == NULL) {
                            printf("Error: Could not open course file.\n");
                        } else {
                            VIEWattendance(coursefile);
                            fclose(coursefile);
                        }
                    } else if (choice == 2) {
                        access = USERstatus(); // Change access
                        break;
                    } else if (choice == 3) {
                        char c;
                        printf("Do you really want to exit? (y/n): ");
                        scanf(" %c", &c); // Space before %c to skip newline
                        if (c == 'y' || c == 'Y') {
                            printf("Thank you for using XYZ Attendance Management System!\n");
                            fclose(dtb);
                            return 0;
                        }
                    } else {
                        printf("Invalid choice. Please try again.\n");
                    }
                    if(access!=1) break;
                }
                break;

            case 2: // Teacher Access
                while (1) {
                    printf("\n--- TEACHER MENU ---\n");
                    printf("1. Add Student\n2. Mark Attendance\n3. View Attendance\n4. Edit Attendance\n");
                    printf("5. Generate Record\n6. Change Access Permissions\n7. Exit\n");
                    printf("Enter your choice: ");
                    int choice;
                    scanf("%d", &choice);
                    while (getchar() != '\n'); // Clear input buffer

                    char course_file[20];
                    FILE *coursefile;

                    if (choice >= 1 && choice <= 5) {
                        printf("Enter Course ID: ");
                        scanf("%15s", course_file);
                        strcat(course_file, ".bin");

                        coursefile = fopen(course_file, (choice == 3) ? "rb" : "rb+");
                        if (coursefile == NULL) {
                            printf("Error: Could not open course file.\n");
                            continue;
                        }
                    }

                    switch (choice) {
                        case 1:
                            ADDstudent(coursefile);
                            break;
                        case 2:
                            MARKattendance(coursefile);
                            break;
                        case 3:
                            VIEWattendance(coursefile);
                            break;
                        case 4:
                            EDITattendance(coursefile);
                            break;
                        case 5:
                            CREATErecord(coursefile);
                            break;
                        case 6:
                            access = USERstatus(); // Change access
                            break;
                        case 7:
                            printf("Thank you for using XYZ Attendance Management System!\n");
                            fclose(dtb);
                            return 0;
                        default:
                            printf("Invalid choice. Please try again.\n");
                    }

                    if (choice >= 1 && choice <= 5) {
                        fclose(coursefile);
                    }
                    if(access!=2) break;
                }
                break;

            case 3: // Admin Access
                while (1) {
                    printf("\n--- ADMIN MENU ---\n");
                    printf("1. Add Course\n2. Add Student\n3. View Attendance\n4. Edit Attendance\n");
                    printf("5. View History\n6. Change Access Permissions\n7. Exit\n");
                    printf("Enter your choice: ");
                    int choice;
                    scanf("%d", &choice);
                    while (getchar() != '\n'); // Clear input buffer

                    char course_file[20];
                    FILE *coursefile;

                    if (choice >= 2 && choice <= 4) {
                        printf("Enter Course ID: ");
                        scanf("%15s", course_file);
                        strcat(course_file, ".bin");

                        coursefile = fopen(course_file, (choice == 3) ? "rb" : "rb+");
                        if (coursefile == NULL) {
                            printf("Error: Could not open course file.\n");
                            continue;
                        }
                    }

                    switch (choice) {
                        case 1:
                            ADDcourse(dtb, nc);
                            break;
                        case 2:
                            ADDstudent(coursefile);
                            break;
                        case 3:
                            VIEWattendance(coursefile);
                            break;
                        case 4:
                            EDITattendance(coursefile);
                            break;
                        case 5:
                            VIEWhistory();
                            break;
                        case 6:
                            access = USERstatus(); // Change access
                            break;
                        case 7:
                            printf("Thank you for using XYZ Attendance Management System!\n");
                            fclose(dtb);
                            return 0;
                        default:
                            printf("Invalid choice. Please try again.\n");
                    }

                    if (choice >= 2 && choice <= 4) {
                        fclose(coursefile);
                    }
                    if(access!=3) break;
                }
                break;

            default:
                printf("Invalid access level. Exiting.\n");
                fclose(dtb);
                return EXIT_FAILURE;
        }
    }

    fclose(dtb);
    return 0;
}
int USERstatus() {
    char password_for_teachers[] = "teacher_access";
    char password_for_admins[] = "admin_access";
    char tpassword[20];
    int access = 0;

    // Open the log file in append mode, create if it doesn't exist
    FILE *logfile = fopen("user_access_log.txt", "a");
    if (logfile == NULL) {
        logfile = fopen("user_access_log.txt", "w");
        exit(EXIT_FAILURE);
    }

    // Ask for the user's name
    char username[50];
    printf("Enter your name: ");
    fgets(username, sizeof(username), stdin);
    username[strcspn(username, "\n")] = '\0';  // Remove trailing newline

    while (!access) {
        int tempac = 0;
        printf("\nACCESS PERMISSIONS\n1) Enter 1 for Student\n2) Enter 2 for Teacher\n3) Enter 3 for Administrator\n");
        printf("Enter your choice: ");
        scanf("%d", &tempac);
        while (getchar() != '\n');  // Clear input buffer

        // Get the current time and date
        time_t now = time(NULL);
        struct tm *t = localtime(&now);
        char timestamp[50];
        strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", t);

        switch (tempac) {
            case 1:  // Student Access
                access = 1;
                fprintf(logfile, "%s | %-15s | Student Access\n", timestamp, username);
                printf("WELCOME, %s! You have Student Access.\n", username);
                break;

            case 2:  // Teacher Access
                printf("Enter password to gain Teacher's access: ");
                scanf("%s", tpassword);
                while (getchar() != '\n');  // Clear input buffer

                if (strcmp(tpassword, password_for_teachers) == 0) {
                    access = 2;
                    fprintf(logfile, "%s | %-15s | Teacher Access\n", timestamp, username);
                    printf("WELCOME, %s! You have Teacher Access.\n", username);
                } else {
                    printf("Incorrect password. Try again.\n");
                }
                break;

            case 3:  // Administrator Access
                printf("Enter password to gain Administrator access: ");
                scanf("%s", tpassword);
                while (getchar() != '\n');  // Clear input buffer

                if (strcmp(tpassword, password_for_admins) == 0) {
                    access = 3;
                    fprintf(logfile, "%s | %-15s | Administrator Access\n", timestamp, username);
                    printf("WELCOME, %s! You have Administrator Access.\n", username);
                } else {
                    printf("Incorrect password. Try again.\n");
                }
                break;

            default:  // Invalid Option
                printf("Invalid option. Please try again.\n");
                break;
        }
    }

    fclose(logfile);  // Close the log file
    return access;
}
void ADDcourse(FILE *dtb, int nc) {
    rewind(dtb); // Move to the start of the file to update course count
    nc++;
    fwrite(&nc, sizeof(int), 1, dtb); // Update course count in the database

    // Allocate memory for the temporary course
    course *tempcourse = (course *)malloc(sizeof(course));
    if (tempcourse == NULL) {
        printf("Error: Memory allocation failed.\n");
        return;
    }

    // Input course details
    printf("Enter Course ID (Max 9 characters): ");
    scanf("%9s", tempcourse->course_id);

    // Check for duplicate Course ID
    course existing_course;
    rewind(dtb); // Move to the start of the file
    fseek(dtb, sizeof(int), SEEK_SET); // Skip the course count
    while (fread(&existing_course, sizeof(course), 1, dtb) == 1) {
        if (strcmp(existing_course.course_id, tempcourse->course_id) == 0) {
            printf("Error: A course with this ID already exists.\n");
            free(tempcourse);
            return;
        }
    }

    printf("Enter Course Name (Max 49 characters): ");
    getchar(); // Clear the input buffer
    fgets(tempcourse->course_name, sizeof(tempcourse->course_name), stdin);

    // Remove trailing newline from course_name, if present
    size_t len = strlen(tempcourse->course_name);
    if (len > 0 && tempcourse->course_name[len - 1] == '\n') {
        tempcourse->course_name[len - 1] = '\0';
    }

    // Save course to the main database file
    fseek(dtb, 0, SEEK_END);
    fwrite(tempcourse, sizeof(course), 1, dtb);

    // Create a binary file for the new course
    char filename[20];
    snprintf(filename, sizeof(filename), "%s.bin", tempcourse->course_id);
    FILE *course_file = fopen(filename, "wb+");
    if (course_file == NULL) {
        printf("Error: Could not create file for course '%s'.\n", tempcourse->course_id);
        free(tempcourse);
        return;
    }

    // Write initial student count (0) to the course file
    int student_count = 0;
    fwrite(&student_count, sizeof(int), 1, course_file);
    fclose(course_file);

    printf("Course '%s' added successfully! Total courses: %d\n", tempcourse->course_name, nc);

    // Free allocated memory
    free(tempcourse);
}
void VIEWcourses(FILE *dtb, int nc) {
    rewind(dtb); // Move to the start of the file
    fseek(dtb, sizeof(int), SEEK_SET); // Skip the course count (nc)

    // Allocate memory for a temporary course
    course *tempcourse = (course *)malloc(sizeof(course));
    if (tempcourse == NULL) {
        printf("Error: Memory allocation failed.\n");
        return;
    }

    // Print total number of courses
    printf("Total number of courses: %d\n\n", nc);

    // Loop through and read each course
    for (int i = 0; i < nc; i++) {
        if (fread(tempcourse, sizeof(course), 1, dtb) != 1) {
            printf("Error: Failed to read course %d from database.\n", i + 1);
            break;
        }
        printf("Course %d:\n", i + 1);
        printf("  Name: %s\n", tempcourse->course_name);
        printf("  ID: %s\n\n", tempcourse->course_id);
    }

    // Free allocated memory for the temporary course
    free(tempcourse);

    // Reset the file pointer to the beginning
    rewind(dtb);
}
void ADDstudent(FILE *coursefile) {
    // Rewind the file and read the current student count
    rewind(coursefile);
    int ns = 0;
    if (fread(&ns, sizeof(int), 1, coursefile) != 1) {
        printf("Error: Failed to read the number of students from the course file.\n");
        return;
    }
    printf("There are currently %d students in this course.\n", ns);

    // Allocate memory for the new student
    student *tempstudent = (student *)malloc(sizeof(student));
    if (tempstudent == NULL) {
        printf("Error: Memory allocation failed.\n");
        return;
    }

    // Get student details from user input
    printf("Enter name (without spaces) and roll number (max 14 characters) of new student: ");
    scanf("%s %14s", tempstudent->name, tempstudent->rollno);

    // Initialize attendance array: Mark all days as 'e' for not marked yet
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 31; j++) {
            tempstudent->att[i][j] = 'e';  // 'e' for not marked yet
        }
    }

    // Move to the end of the file and write the new student
    fseek(coursefile, 0, SEEK_END);
    if (fwrite(tempstudent, sizeof(student), 1, coursefile) != 1) {
        printf("Error: Failed to write student data to the course file.\n");
        free(tempstudent);
        return;
    }

    // Update the number of students (increment it by 1)
    ns++;

    // Go back to the start of the file to write the updated student count
    rewind(coursefile);
    if (fwrite(&ns, sizeof(int), 1, coursefile) != 1) {
        printf("Error: Failed to update student count in the course file.\n");
        free(tempstudent);
        return;
    }

    // Free the allocated memory
    free(tempstudent);

    printf("Student added successfully.\n");
}
void MARKattendance(FILE *coursefile) {
    rewind(coursefile);
    int ns = 0;
    fread(&ns, sizeof(int), 1, coursefile);  // Read the number of students

    student *tempstudent = (student *)malloc(sizeof(student));
    if (tempstudent == NULL) {
        printf("Memory allocation failed.\n");
        return;
    }

    for (int i = 0; i < ns; i++) {
        // Move to the correct position for student i
        fseek(coursefile, sizeof(int) + i * sizeof(student), SEEK_SET);  // Skip the student count and go to the student record
        fread(tempstudent, sizeof(student), 1, coursefile);  // Read the student record

        // Loop through months and days to mark attendance
        for (int month = 0; month < 4; month++) {
            int done = 0;
            for (int day = 0; day < 31; day++) {
                if (tempstudent->att[month][day] == 'e') {  // If the attendance is not yet marked
                    char attendance;
                    printf("For student %s (Roll No: %s), enter 'p' for present or 'a' for absent: ", tempstudent->name, tempstudent->rollno);
                    // Clear the buffer before using scanf
                    while ((getchar()) != '\n');  // Consume any leftover newline characters
                    scanf("%c", &attendance);  // Get attendance input

                    // Validate the attendance input
                    if (attendance == 'p' || attendance == 'a') {
                        tempstudent->att[month][day] = attendance;  // Update attendance
                    } else {
                        printf("Invalid input! Attendance not recorded for student %s.\n", tempstudent->name);
                    }
                    done++;
                    break;
                }
            }
            if (done) break;  // If attendance for this student is marked, break out of the loop
        }

        // Move file pointer back to the correct position to overwrite the record
        fseek(coursefile, sizeof(int) + i * sizeof(student), SEEK_SET);  // Skip the student count and go to the student record
        fwrite(tempstudent, sizeof(student), 1, coursefile);  // Write the updated student record
    }

    free(tempstudent);
    printf("Attendance for the specified day has been marked.\n");
    rewind(coursefile);
}
void EDITattendance(FILE *coursefile) {
    rewind(coursefile);
    int ns = 0;
    fread(&ns, sizeof(int), 1, coursefile);  // Read the number of students

    student *tempstudent = (student *)malloc(sizeof(student));
    if (tempstudent == NULL) {
        printf("Memory allocation failed.\n");
        return;
    }

    // Get the roll number of the student whose attendance needs to be updated
    char rollno_to_update[10];
    printf("Enter the roll number of the student whose attendance you want to update: ");
    scanf("%s", rollno_to_update);

    int found = 0;  // Flag to check if student is found
    for (int i = 0; i < ns; i++) {
        // Move to the correct position for student i
        fseek(coursefile, sizeof(int) + i * sizeof(student), SEEK_SET);  // Skip the student count and go to the student record
        fread(tempstudent, sizeof(student), 1, coursefile);  // Read the student record

        // Check if the current student matches the roll number
        if (strcmp(tempstudent->rollno, rollno_to_update) == 0) {
            found = 1;  // Student found
            printf("Found student %s (Roll No: %s)\n", tempstudent->name, tempstudent->rollno);

            // Asking user for month and day to edit attendance
            int month = 0, day = 0;
            printf("Enter the date (1-31) and month (1-4) to edit: ");
            scanf("%d %d", &day, &month);

            // Validate the input for day and month
            if (month < 1 || month > 4 || day < 1 || day > 31) {
                printf("Invalid date or month input. Aborting attendance update.\n");
                free(tempstudent);
                break;
            }

            // Display current attendance status for the specified date
            printf("This student is marked '%c' for this day.\n", tempstudent->att[month - 1][day - 1]);

            // Prompt for new attendance status
            char natt = 'e';
            printf("Enter new data ('a' for absent, 'p' for present): ");
            while ((getchar()) != '\n');  // Clear input buffer
            scanf("%c", &natt);

            // Validate new attendance status
            if (natt != 'a' && natt != 'p') {
                printf("Invalid attendance input. Aborting update.\n");
                free(tempstudent);
                break;
            }

            // Update attendance
            tempstudent->att[month - 1][day - 1] = natt;

            // Move file pointer back to the student position to overwrite the record
            fseek(coursefile, sizeof(int) + i * sizeof(student), SEEK_SET);  // Skip the student count and go to the student record
            fwrite(tempstudent, sizeof(student), 1, coursefile);

            printf("Attendance has been successfully updated for student %s (Roll No: %s).\n", tempstudent->name, tempstudent->rollno);
            free(tempstudent);
            return;
            break;  // Exit the loop once the attendance is updated
        }
    }

    // If the student was not found
    if (!found) {
        printf("Student with Roll No %s not found.\n", rollno_to_update);
    }

    free(tempstudent);  // Ensure memory is freed even if student is not found
    return;
    rewind(coursefile);
}
void VIEWattendance(FILE *coursefile) {
    rewind(coursefile);

    // Get day and month from user
    int month = 0, day = 0;
    printf("Enter the day (1-31) and month (1-4) to view attendance: ");
    scanf("%d %d", &day, &month);

    // Validate input
    if (month < 1 || month > 4 || day < 1 || day > 31) {
        printf("Invalid day or month input. Please enter valid values.\n");
        return;
    }

    int ns = 0;  // Number of students
    if (fread(&ns, sizeof(int), 1, coursefile) != 1) {
        printf("Error: Failed to read the number of students from the course file.\n");
        return;
    }

    printf("Attendance records for Day %d, Month %d:\n", day, month);

    // Allocate memory for a temporary student
    student *tempstudent = (student *)malloc(sizeof(student));
    if (tempstudent == NULL) {
        printf("Memory allocation failed.\n");
        return;
    }

    // Read attendance for each student
    for (int i = 0; i < ns; i++) {
        // Ensure we are reading the student record properly
        if (fread(tempstudent, sizeof(student), 1, coursefile) != 1) {
            printf("Error: Failed to read student data.\n");
            break;
        }

        // Display attendance for the specified day and month
        printf("%s (%s): %c\n", tempstudent->name, tempstudent->rollno, tempstudent->att[month - 1][day - 1]);
    }

    // Free allocated memory
    free(tempstudent);

    // Reset the file pointer to the beginning of the file
    rewind(coursefile);
}
void VIEWhistory() {
    // Open the log file in read mode
    FILE *logFile = fopen("user_access_log.txt", "r");
    if (logFile == NULL) {
        printf("No history found. The log file does not exist.\n");
        return;
    }

    // Print the access history header
    printf("---- Access History ----\n");

    // Read and print each line of the log file
    char line[256];
    while (fgets(line, sizeof(line), logFile) != NULL) {
        printf("%s", line); // Print each line of the log file
    }

    // Close the file
    fclose(logFile);

    // Print the end of history message
    printf("---- End of History ----\n");
}
void CREATErecord(FILE *coursefile) {
    rewind(coursefile);
    int ns = 0;
    fread(&ns, sizeof(int), 1, coursefile);  // Read the number of students

    int month = 0, startday = 0, endday = 0;
    printf("Enter month(1-4) and start(1-31) and end day of the desired record: ");
    scanf("%d %d %d", &month, &startday, &endday);

    // Validate month, start day, and end day input
    if (month < 1 || month > 4) {
        printf("Invalid month. Please enter a month between 1 and 4.\n");
        return;
    }
    if (startday < 1 || startday > 31 || endday < 1 || endday > 31 || startday > endday) {
        printf("Invalid day range. Please enter valid start and end days between 1 and 31.\n");
        return;
    }

    // Safe file name input
    char file_name[25];  // Increased size to safely accommodate file name
    printf("Enter the name of the text file you want the attendance record in (max 15 characters): ");
    scanf("%24s", file_name);
    snprintf(file_name + strlen(file_name), sizeof(file_name) - strlen(file_name), ".txt");  // Safely append .txt

    FILE *record = fopen(file_name, "w");
    if (record == NULL) {
        printf("Error opening the file for writing.\n");
        return;
    }

    // Write the header row with the dates
    fprintf(record, "|%-15s|", "NAME");
    for (int i = startday; i <= endday; i++) {
        fprintf(record, " %2d |", i);  // Write day
    }
    fprintf(record, "\n");

    // Allocate memory for student data and write attendance
    for (int i = 0; i < ns; i++) {
        student *tempstudent = (student *)malloc(sizeof(student));
        if (tempstudent == NULL) {
            printf("Memory allocation failed for student %d\n", i + 1);
            continue;
        }

        // Read student data from coursefile
        fread(tempstudent, sizeof(student), 1, coursefile);

        // Write student name and attendance for each day
        fprintf(record, "|%-15s|", tempstudent->name);
        for (int day = startday; day <= endday; day++) {
            if (day >= 1 && day <= 31) {
                fprintf(record, "  %c |", tempstudent->att[month - 1][day - 1]);  // Ensure attendance data is valid
            }
        }
        fprintf(record, "\n");

        free(tempstudent);  // Free allocated memory
    }

    fclose(record);
    printf("A file named %s is created in your working directory.\n", file_name);
}
///////////   ~~~THE END~~~   /////////////55