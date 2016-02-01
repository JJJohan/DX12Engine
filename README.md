[![Stories in Ready](https://badge.waffle.io/JJJohan/DX12Engine.png?label=ready&title=Ready)](https://waffle.io/JJJohan/DX12Engine)
# DirectX 12 Engine

## What is this? ##
This repository is a personal hobby project of mine to investigate and learn about the new DirectX 12 framework released by Microsoft in the hopes of improving my understanding of various design concepts, general programming knowledge, newer C++11+ concepts as well as traditional ones. This serves as a side project to my existing 40 hours a week of full-time C# work.

## What can it do? ##
At the time of writing the project is quite its early stages but it's coming along nicely with the intention of keeping the code well structured and more importantly keeping it both fast and stable.

The engine itself is capable of drawing 'Render Objects' which serve as the basis of anything drawn on screen. These objects micro manage other necessary components such as index and vertex buffers, materials which in turn manage attached textures and so on.

## What is your goal? ##

Originally I was interested in creating a deferred renderer as this is not something I've done before, and still intend to do so. Firstly my aim is to produce a core with a sufficient feature set to be able to perform a series of tasks you might expect to be able to perform with a simple rendering system.

There isn't necessarily a primary goal or a linear path I'm taking towards the ideas I have in mind, and often I will find myself adding something and then finding out about something similar, or even a completely different idea and working my way towards that. An example of this is DirectX 12's parallel powers and have put some effort into creating a parallel command queue.

Rather than having to update the readme at a similar frequency as commits I invite anyone interested to have a look for themselves, even if only at the project's milestones to get an idea of the progress and features both supported and planned.
