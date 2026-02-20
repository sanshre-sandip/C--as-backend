CREATE DATABASE IF NOT EXISTS portfolio_db;
USE portfolio_db;

CREATE TABLE IF NOT EXISTS posts (
    id INT AUTO_INCREMENT PRIMARY KEY,
    title VARCHAR(255) NOT NULL,
    content TEXT NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE IF NOT EXISTS projects (
    id INT AUTO_INCREMENT PRIMARY KEY,
    title VARCHAR(255) NOT NULL,
    description TEXT,
    url VARCHAR(255),
    image_url VARCHAR(255)
);

-- Seed Data
INSERT INTO posts (title, content) VALUES 
('First Post', 'Welcome to my C-powered blog!'),
('Learning C', 'Memory management is fun... usually.');

INSERT INTO projects (title, description, url, image_url) VALUES 
('Portfolio Site', 'This website, built with C and React', 'http://localhost:5173', 'https://via.placeholder.com/150'),
('Project Alpha', 'A cool C++ game engine', 'https://github.com/example/alpha', 'https://via.placeholder.com/150');
