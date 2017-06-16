#include "neighborhoodsorter.h"
#include "detect_events.h"
#include "dimension_reduce_clips.h"
#include "sort_clips.h"
#include "consolidate_clusters.h"

#include <QMutexLocker>
#include <mda.h>

class NeighborhoodSorterPrivate {
public:
    NeighborhoodSorter *q;
    QList<bigint> m_channels;
    P_multineighborhood_sort_opts m_opts;
    QVector<double> m_timepoints;
    QVector<int> m_labels;
    Mda32 m_clips;
    Mda32 m_reduced_clips;
    Mda m_templates;
    QMutex m_mutex;
};

NeighborhoodSorter::NeighborhoodSorter()
{
    d=new NeighborhoodSorterPrivate;
    d->q=this;
}

NeighborhoodSorter::~NeighborhoodSorter()
{
    delete d;
}

void NeighborhoodSorter::setChannels(const QList<bigint> &channels)
{
    d->m_channels=channels;
}

QList<bigint> NeighborhoodSorter::channels() const
{
    return d->m_channels;
}

void NeighborhoodSorter::setOptions(const P_multineighborhood_sort_opts &opts)
{
    d->m_opts=opts;
}

void NeighborhoodSorter::chunkDetect(const NeighborhoodChunk &chunk)
{
    QVector<double> data0(chunk.data.N2());
    for (bigint i=0; i<chunk.data.N2(); i++) {
        double val=chunk.data.value(0,i);
        data0[i]=val;
    }
    QVector<double> timepoints=detect_events(data0,d->m_opts.detect_threshold,d->m_opts.detect_interval,d->m_opts.detect_sign);
    {
        QMutexLocker locker(&d->m_mutex);
        for (bigint i=0; i<timepoints.count(); i++) {
            double t0=timepoints[i]-chunk.t_offset+chunk.t1;
            if ((chunk.t1<=t0)&&(t0<chunk.t2+1)) {
                d->m_timepoints << t0;
            }
        }
    }
}

void NeighborhoodSorter::initializeExtractClips()
{
    qSort(d->m_timepoints);
    bigint M2=d->m_channels.count();
    int T=d->m_opts.clip_size;
    bigint L=d->m_timepoints.count();
    d->m_clips.allocate(M2,T,L);
}

void NeighborhoodSorter::chunkExtractClips(const NeighborhoodChunk &chunk)
{
    int T=d->m_opts.clip_size;
    int Tmid = (int)((T + 1) / 2) - 1;
    int M2=chunk.data.N1();
    bigint iii=0;
    while ((iii<d->m_timepoints.count())&&(d->m_timepoints[iii]<chunk.t1)) {
        iii++;
    }
    while ((iii<d->m_timepoints.count())&&(d->m_timepoints[iii]<chunk.t2+1)) {
        bigint t0=d->m_timepoints[iii]-chunk.t1+chunk.t_offset;
        Mda32 tmp;
        chunk.data.getChunk(tmp,0,t0-Tmid,M2,T);
        {
            QMutexLocker locker(&d->m_mutex); //is this necessary?
            d->m_clips.setChunk(tmp,0,0,iii);
        }
        iii++;
    }
}

void NeighborhoodSorter::reduceClips()
{
    //d->m_reduced_clips.write32("/home/magland/tmp/test_clips.mda");
    dimension_reduce_clips(d->m_reduced_clips,d->m_clips,d->m_opts.num_features_per_channel,d->m_opts.max_pca_samples);
}

void NeighborhoodSorter::sortClips()
{
    Sort_clips_opts oo;
    oo.max_samples=d->m_opts.max_pca_samples;
    oo.num_features=d->m_opts.num_features;
    d->m_labels=sort_clips(d->m_reduced_clips,oo);
}

void NeighborhoodSorter::computeTemplates()
{
    int M2=d->m_channels.count();
    int T=d->m_opts.clip_size;
    int K=MLCompute::max(d->m_labels);
    d->m_templates.allocate(M2,T,K);
    QVector<double> label_counts(K,0);
    for (bigint i=0; i<d->m_labels.count(); i++) {
        int k0=d->m_labels[i];
        if (k0>0) {
            label_counts[k0-1]++;
            Mda32 tmp;
            d->m_clips.getChunk(tmp,0,0,i,M2,T,1);
            for (int t=0; t<T; t++) {
                for (int m=0; m<M2; m++) {
                    d->m_templates.set(d->m_templates.get(m,t,k0-1)+tmp.get(m,t),m,t,k0-1);
                }
            }
        }

    }
    for (int k=1; k<=K; k++) {
        double denom=label_counts[k-1];
        if (!denom) denom=1;
        for (int t=0; t<T; t++) {
            for (int m=0; m<M2; m++) {
                d->m_templates.set(d->m_templates.get(m,t,k-1)/denom,m,t,k-1);
            }
        }
    }
}

Mda NeighborhoodSorter::templates() const
{
    return d->m_templates;
}

void NeighborhoodSorter::consolidateClusters()
{
    int M2=d->m_channels.count();
    int T=d->m_opts.clip_size;
    QVector<bigint> event_inds;
    Consolidate_clusters_opts oo;
    consolidate_clusters(event_inds,d->m_timepoints,d->m_labels,d->m_templates,oo);
    bigint L_new=d->m_timepoints.count();

    {
        Mda32 new_clips(M2,T,L_new);
        for (bigint i=0; i<L_new; i++) {
            Mda32 tmp;
            d->m_clips.getChunk(tmp,0,0,event_inds[i],M2,T,1);
            new_clips.setChunk(tmp,0,0,i);
        }
        d->m_clips=new_clips;
    }

    {
        Mda32 new_reduced_clips(M2,d->m_opts.num_features_per_channel,L_new);
        for (bigint i=0; i<L_new; i++) {
            Mda32 tmp;
            d->m_reduced_clips.getChunk(tmp,0,0,event_inds[i],M2,d->m_opts.num_features_per_channel,1);
            new_reduced_clips.setChunk(tmp,0,0,i);
        }
        d->m_reduced_clips=new_reduced_clips;
    }

    this->computeTemplates();
}
