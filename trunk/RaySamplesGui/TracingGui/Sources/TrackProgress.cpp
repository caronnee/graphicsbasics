#include "TrackProgress.h"

void TrackProgress::Update()
{
	_progress += _step;
	emit Progress(_progress*100);
}

void TrackProgress::Clear()
{
	_progress = 0;
	emit Progress(_progress);
}

void TrackProgress::SetStages(float stages)
{
	_step = 1/stages;
}

