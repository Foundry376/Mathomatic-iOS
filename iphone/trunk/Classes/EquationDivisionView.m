//
//  EquationDivisionView.m
//  mathomatic
//
//  Created by Ben Gotow on 11/14/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "EquationDivisionView.h"


@implementation EquationDivisionView

- (id)initWithParent:(EquationContainerView*)p {
    if (self = [super initWithParent: p]) {
        innerEquationsMax = 2;
        alignmentMethod = kAlignmentMethodCentered;
    }
    return self;
}

- (void)drawRect:(CGRect)rect {
    if ([innerEquations count] > 0){
        CGRect          r = [[innerEquations objectAtIndex: 0] frame];
        CGContextRef    c = UIGraphicsGetCurrentContext();
        CGRect          d = CGRectMake(0, r.size.height + 2, [self frame].size.width, 1);
        CGContextFillRect(c, d);
    }
}

- (void)finalizeEquationHierarchy
{
    for (int ii = [self.innerEquations count] - 1; ii >= 0; ii --)
        [[innerEquations objectAtIndex: ii] finalizeEquationHierarchy];

    EquationAbstractView * next;
    int index = [parent.innerEquations indexOfObjectIdenticalTo: self];
    
    if (index + 1 < [parent.innerEquations count]){
        next = [parent.innerEquations objectAtIndex: index + 1];
        if (next){
            [self addInnerEquation: next];
            [parent.innerEquations removeObjectIdenticalTo: next];
        }
    }
}

- (void)finalizeFrame
{
    [innerEquations makeObjectsPerformSelector:@selector(finalizeFrame)];
    
    if ([innerEquations count] == 2){
        EquationAbstractView * a = [innerEquations objectAtIndex: 0];
        EquationAbstractView * b = [innerEquations objectAtIndex: 1];
        
        int width = fmax([a frame].size.width, [b frame].size.width);
        
        CGRect aFrame = CGRectMake((width-[a frame].size.width) / 2, 0, [a frame].size.width, [a frame].size.height);
        CGRect bFrame = CGRectMake((width-[b frame].size.width) / 2, aFrame.size.height, [b frame].size.width, [b frame].size.height);
        [a setFrame: aFrame];
        [b setFrame: bFrame];
        
        [self setFrame: CGRectMake(0,0, width, aFrame.size.height + bFrame.size.height)];
    } else {
        [super finalizeFrame];
    }
}

- (NSString*)description
{
    if ([innerEquations count] == 2)
        return [NSString stringWithFormat:@"%@/%@ [frame=%f,%f,%f,%f]", [[self.innerEquations objectAtIndex: 0] description]
                                                  , [[self.innerEquations objectAtIndex: 1] description]
                                                  , [self frame].origin.x
                                                  , [self frame].origin.y
                                                  , [self frame].size.width
                                                  , [self frame].size.height];
    else
        return @"Incomplete Division";
}

- (void)dealloc {
    [super dealloc];
}


@end
