/**
 * Copyright  (C) 2013 IIT - Istituto Italiano di Tecnologia
 * Author: Silvio Traversaro
 * website: http://www.codyco.eu
 */
 
#include "kdl_codyco/position_loops.hpp"

namespace KDL {
namespace CoDyCo {

    int getFrameLoop(const UndirectedTree & undirected_tree,
                const KDL::JntArray &q, 
                const Traversal & traversal,
                const int proximal_link_index,
                const int distal_link_index,
                Frame & frame_proximal_distal)
    {
        LinkMap::const_iterator distal_it = undirected_tree.getLink(distal_link_index);
        LinkMap::const_iterator proximal_it = undirected_tree.getLink(proximal_link_index);
        
        Frame currentFrame;
        Frame resultFrame = Frame::Identity();
        for(LinkMap::const_iterator link=distal_it; link != proximal_it; link = traversal.getParentLink(link) ) {
            LinkMap::const_iterator parent_link = traversal.getParentLink(link);
            assert( parent_link != undirected_tree.getInvalidLinkIterator() );
            
            double joint_position;
            
            if( link->getAdjacentJoint(parent_link)->getJoint().getType() != Joint::None ) {
                joint_position = q((link->getAdjacentJoint(parent_link))->getDOFIndex());
            } else {
                joint_position =0;
            }
            
            currentFrame = link->pose(parent_link,
                                             joint_position);
                                             
            resultFrame = currentFrame*resultFrame;
        }
        
        frame_proximal_distal = resultFrame;
    
        return 0;
    }
    
    Frame getFrameLoop(const UndirectedTree & undirected_tree,
                const KDL::JntArray &q, 
                const Traversal & traversal,
                const int proximal_link_index,
                const int distal_link_index)
    {
        Frame frame_proximal_distal;
        getFrameLoop(undirected_tree,q,traversal,proximal_link_index,distal_link_index,frame_proximal_distal);
        return frame_proximal_distal;
    }
    
    
    int getFramesLoop(const UndirectedTree & undirected_tree,
                      const KDL::JntArray &q, 
                      const Traversal & traversal,
                      std::vector<Frame> & X_base)
    {
          for(int i=0; i < (int)traversal.getNrOfVisitedLinks(); i++) {
            double joint_pos;
            LinkMap::const_iterator link_it = traversal.getOrderedLink(i);
            int link_nmbr = link_it->getLinkIndex(); 
            if( i == 0 ) {
                assert( traversal.getParentLink(link_nmbr) == undirected_tree.getInvalidLinkIterator() );
                X_base[link_nmbr] = KDL::Frame::Identity();
            } else {
                LinkMap::const_iterator parent_it = traversal.getParentLink(link_it);
                int parent_nmbr = parent_it->getLinkIndex();
                
                if( link_it->getAdjacentJoint(parent_it)->getJoint().getType() != Joint::None ) {
                    int dof_nr = link_it->getAdjacentJoint(parent_it)->getDOFIndex();
                    joint_pos = q(dof_nr);
                } else {
                    joint_pos =  0.0;
                }
                KDL::Frame X_parent_son = link_it->pose(parent_it,joint_pos);
                X_base[link_nmbr] = X_base[parent_nmbr]*X_parent_son;
            }
        }
        return 0;
    }
}
}
