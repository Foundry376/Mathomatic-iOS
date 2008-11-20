//
//  EquationAbstractView.h
//  mathomatic
//
//  Created by Ben Gotow on 11/14/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>

#define kAlignmentMethodBottom  0
#define kAlignmentMethodCentered  1


@interface EquationAbstractView : UIView {
    int alignmentMethod;
}

@property (nonatomic, assign) int alignmentMethod;

- (void)finalizeEquationHierarchy;
- (void)finalizeTextSize;

@end
