# MOVIE-BOOKING-SYSTEM
"A A robust and transactional Movie Ticket Reservation System System featuring a 3NF normalized relational schema. Implemented with C++ and Oracle 21c (OCCI), the system utilizes row-level locking for ACID-compliant transactions, PL/SQL triggers for automated inventory management, and stored procedures for resource allocation."

# Movie-Ticket-Reservation: High-Concurrency Ticket Booking System

A robust Movie Ticket Reservation System built with **C++** and **Oracle 21c (OCCI)**, designed to handle multi-theater operations with strict ACID compliance.

## Key DBMS Features (Syllabus Aligned)
- **3NF Relational Schema:** Designed 7 normalized tables to eliminate data redundancy.
- **Concurrency Control:** Implemented Pessimistic Locking (`SELECT FOR UPDATE`) to handle simultaneous booking attempts.
- **Database Automation:** Utilized **PL/SQL Triggers** for automated seat status updates and **Stored Procedures** for batch seat generation.
- **Complex Querying:** Multi-table JOINS and Aggregate functions for real-time revenue reporting.

##  Tech Stack
- **Language:** C++ (Standard 17)
- **Database:** Oracle Database 21c Express Edition (XE)
- **API:** Oracle C++ Call Interface (OCCI)

##  Testing & Results
The following test cases demonstrate the system's ability to handle transactions and multi-theater management:

### Test Case 1: Standard Booking & Trigger Verification
- **Input:** Booked Seat `100101` for User `500`.
- **Observation:** `Ticket ID: 1` generated. Seat status automatically shifted to `[BOOKED]` via DB Trigger.
- **Revenue:** Admin Report for Theater 10 updated to Rs. 350.

### Test Case 2: Multi-Theater Management
- **Input:** Logged in as Admin for Theater 20 (INOX Delhi). Added Movie "Batman" and scheduled Show `2001`.
- **Result:** Show became available globally, but revenue tracking remained isolated to Theater 20.

### Test Case 3: Cancellation & Revenue Reversal
- **Action:** Cancelled Ticket `1`.
- **Result:** `trg_cancel_seat` Trigger restored seat status to Available. Revenue Report accurately reflected Rs. 0.
