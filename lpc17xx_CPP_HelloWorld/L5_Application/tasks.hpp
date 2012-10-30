/**
 * @file tasks.hpp
 * @brief Contains FreeRTOS Tasks
 */
#ifndef TASKS_HPP_
#define TASKS_HPP_



/// Terminal Task is defined at terminal.cpp
void terminalTask(void *p);

/// Task that monitors switches and lights up LEDs
void switchled(void* p);



#endif /* TASKS_HPP_ */
