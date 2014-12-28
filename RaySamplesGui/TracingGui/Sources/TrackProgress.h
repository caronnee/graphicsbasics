#ifndef __TRACK_PROGRESS__
#define __TRACK_PROGRESS__

#include <QObject>

class TrackProgress : public QObject
{
	Q_OBJECT

private:
	float _progress;
	float _step;
signals:
	void Progress(int value);
public:
	TrackProgress(): _progress(0),_step(1) {  }
	void Update();
	void Clear();
	void SetStages(float stages);
  void Finish();
};

#endif