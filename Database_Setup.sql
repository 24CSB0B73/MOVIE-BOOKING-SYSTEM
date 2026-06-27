-- 1. Create Tables under movieuser
CREATE TABLE Movies (movie_id INT PRIMARY KEY, title VARCHAR2(100), duration_mins INT);
CREATE TABLE Theaters (theater_id INT PRIMARY KEY, name VARCHAR2(100), location VARCHAR2(100));
CREATE TABLE Uusers (user_id INT PRIMARY KEY, username VARCHAR2(50), email VARCHAR2(100));
CREATE TABLE Shows (show_id INT PRIMARY KEY, movie_id INT REFERENCES Movies(movie_id), theater_id INT REFERENCES Theaters(theater_id), show_time TIMESTAMP, price NUMBER(10,2));
CREATE TABLE Seats (seat_id INT PRIMARY KEY, show_id INT REFERENCES Shows(show_id), seat_number VARCHAR2(10), status INT DEFAULT 0);
CREATE TABLE Bookings (booking_id INT GENERATED ALWAYS AS IDENTITY PRIMARY KEY, user_id INT REFERENCES Uusers(user_id), show_id INT REFERENCES Shows(show_id), booking_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP, total_amount NUMBER(10,2));
CREATE TABLE Tickets (ticket_id INT GENERATED ALWAYS AS IDENTITY PRIMARY KEY, booking_id INT REFERENCES Bookings(booking_id), seat_id INT REFERENCES Seats(seat_id));

-- 2. Insert Dummy Data
INSERT INTO Movies VALUES (1, 'Inception', 148);
INSERT INTO Movies VALUES (2, 'Interstellar', 169);
INSERT INTO Theaters VALUES (10, 'PVR Mumbai', 'Andheri');
INSERT INTO Theaters VALUES (20, 'INOX Delhi', 'Connaught Place');
INSERT INTO Uusers VALUES (500, 'rahul_99', 'rahul@email.com');
INSERT INTO Shows VALUES (1001, 1, 10, TO_TIMESTAMP('2024-06-10 18:00:00', 'YYYY-MM-DD HH24:MI:SS'), 350.00);
INSERT INTO Seats VALUES (7001, 1001, 'A1', 0);
INSERT INTO Seats VALUES (7002, 1001, 'A2', 0);

COMMIT;
CREATE OR REPLACE TRIGGER update_seat_booked
AFTER INSERT ON Tickets
FOR EACH ROW
BEGIN
    UPDATE Seats SET status = 1 WHERE seat_id = :NEW.seat_id;
END;
/
SELECT u.username, m.title, s.seat_number, b.total_amount, b.booking_date
FROM Bookings b
JOIN Uusers u ON b.user_id = u.user_id
JOIN Tickets t ON b.booking_id = t.booking_id
JOIN Seats s ON t.seat_id = s.seat_id
JOIN Shows sh ON s.show_id = sh.show_id
JOIN Movies m ON sh.movie_id = m.movie_id;